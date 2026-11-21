import argparse
from ctypes import c_float
from pathlib import Path
from struct import Struct


OUTPUT_DIR = Path(__file__).resolve().parents[1] / "output" / "octree"
OCTREE_HEADER = Struct("<4sIIf")
OCTREE_SOURCE = Struct("<4f")


def load_sources(filename):
    points = []

    with open(filename) as f:
        for line in f:
            if line.startswith("#"):
                continue

            x, y, z, m = map(float, line.split())
            points.append((x, y, z, m))

    return points


def load_octree(filename):
    nodes = []

    with open(filename) as f:
        for line in f:
            if line.startswith("#"):
                continue

            values = line.split()

            node_id = int(values[0])
            parent_id = int(values[1])

            x = float(values[2])
            y = float(values[3])
            z = float(values[4])

            total_mass = float(values[5])
            half_size = float(values[6])
            leaf = int(values[7])

            nodes.append(
                (x, y, z, total_mass, half_size, leaf)
            )

    return nodes


def load_binary_octree(filename):
    """Read OCT1 nodes and sources in the same layout as the text readers."""
    data = Path(filename).read_bytes()
    if len(data) < OCTREE_HEADER.size:
        raise ValueError("Incomplete OCT1 header")
    magic, split_count, source_count, root_half_size = OCTREE_HEADER.unpack_from(data)
    if magic != b"OCT1":
        raise ValueError("Unsupported octree binary format")

    mask_end = OCTREE_HEADER.size + split_count
    if len(data) != mask_end + source_count * OCTREE_SOURCE.size:
        raise ValueError("OCT1 file size does not match its header")

    nodes = []
    children = {}
    next_mask = OCTREE_HEADER.size

    def add_node(x, y, z, half_size, splits):
        nonlocal next_mask
        index = len(nodes)
        nodes.append([x, y, z, 0.0, half_size, int(not splits)])
        if splits:
            if next_mask == mask_end:
                raise ValueError("Missing OCT1 child mask")
            mask = data[next_mask]
            next_mask += 1
            h = c_float(half_size / 2).value
            child_indices = []
            for octant in range(8):
                child_indices.append(add_node(
                    c_float(x + (h if octant & 1 else -h)).value,
                    c_float(y + (h if octant & 2 else -h)).value,
                    c_float(z + (h if octant & 4 else -h)).value,
                    h,
                    mask & (1 << octant),
                ))
            children[index] = child_indices
        return index

    add_node(0.0, 0.0, 0.0, root_half_size, split_count != 0)
    if next_mask != mask_end:
        raise ValueError("Unused OCT1 child masks")

    sources = list(OCTREE_SOURCE.iter_unpack(memoryview(data)[mask_end:]))
    for x, y, z, mass in sources:
        index = 0
        while True:
            node = nodes[index]
            node[3] = c_float(node[3] + mass).value
            if node[5]:
                break
            octant = (x >= node[0]) | ((y >= node[1]) << 1) | ((z >= node[2]) << 2)
            index = children[index][octant]

    return nodes, sources


def draw_cube(ax, center, half_size, leaf):
    x, y, z = center
    h = half_size
    
    vertices = [
        (x-h, y-h, z-h),
        (x+h, y-h, z-h),
        (x+h, y+h, z-h),
        (x-h, y+h, z-h),
        (x-h, y-h, z+h),
        (x+h, y-h, z+h),
        (x+h, y+h, z+h),
        (x-h, y+h, z+h),
    ]

    edges = [
        (0,1), (1,2), (2,3), (3,0),
        (4,5), (5,6), (6,7), (7,4),
        (0,4), (1,5), (2,6), (3,7)
    ]

    for i, j in edges:
        ax.plot(
            [vertices[i][0], vertices[j][0]],
            [vertices[i][1], vertices[j][1]],
            [vertices[i][2], vertices[j][2]],
            color='black', alpha=0.5,
            linewidth=0.3
        )


def visualize(prefix):
    import matplotlib.pyplot as plt

    path = Path(prefix)
    binary = path if path.suffix == ".bin" else OUTPUT_DIR / f"{prefix}.bin"
    if path.suffix == ".bin" or binary.is_file():
        nodes, sources = load_binary_octree(binary)
    else:
        nodes = load_octree(OUTPUT_DIR / f"{prefix}_data.txt")
        sources = load_sources(OUTPUT_DIR / f"{prefix}_sources.txt")

    fig = plt.figure()
    ax = fig.add_subplot(
        111,
        projection="3d"
    )

    # Draw tree cells
    for x, y, z, mass, h, leaf in nodes:
        if mass < 0.1 or not leaf:
            continue
        draw_cube(
            ax,
            (x, y, z),
            h,
            leaf
        )
        
    if sources:
        xs, ys, zs, masses = zip(*sources)
        sc = ax.scatter(
            xs, ys, zs,
            c=masses,
            cmap='coolwarm',
            norm='log' if min(masses) > 0 else 'linear',
            s=2,
            rasterized=True,
        )
        plt.colorbar(sc, ax=ax)
    elif nodes:
        x, y, z, _, h, leaf = nodes[0]
        draw_cube(ax, (x, y, z), h, leaf)

    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")

    plt.savefig(binary.with_name(f"{binary.stem}_visualization.png"),
                dpi=300, bbox_inches='tight', pad_inches=0.05)
    plt.close(fig)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Render octree leaves and sources.")
    parser.add_argument("tree", nargs="*", default=["biteau"],
                        help="output prefix or path to an OCT1 .bin file")
    for tree in parser.parse_args().tree:
        visualize(tree)
