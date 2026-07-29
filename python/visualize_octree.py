import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


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


def visualize(
    prefix
):
    folder = "output/octree/"
    nodes = load_octree(folder + f"{prefix}_data.txt")

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
        
    sources = load_sources(folder + f"{prefix}_sources.txt")

    # Draw sources
    xs = [p[0] for p in sources]
    ys = [p[1] for p in sources]
    zs = [p[2] for p in sources]
    ms = [p[3] for p in sources]

    sc = ax.scatter(
        xs,
        ys,
        zs,
        # c=ms,
        # cmap='coolwarm',
        # norm='log',
        s=2,
        rasterized=True
    )
    
    plt.colorbar(sc, ax=ax)


    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")

    plt.savefig(f"output/octree/{prefix}_visualization.png",
                dpi=300, bbox_inches='tight', pad_inches=0.05)


if __name__ == "__main__":
    visualize(
        "biteau"
    )