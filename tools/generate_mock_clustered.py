"""Generate a mock octree catalog with three Gaussian clusters on the x-axis."""

from random import Random

from generate_mock_uniform import parse_catalog_args, uniform_point, write_catalog


def gaussian_point(rng, center_x, sigma, half_size):
    while True:
        point = (
            rng.gauss(center_x, sigma),
            rng.gauss(0, sigma),
            rng.gauss(0, sigma),
        )
        if all(abs(value) <= half_size for value in point):
            return point


def main():
    args = parse_catalog_args(__doc__, "mock_clustered.txt")
    rng = Random(args.seed)
    half_size = args.cube_size_mpc / 2
    sigma = args.cube_size_mpc / 10
    centers_x = (-0.3 * args.cube_size_mpc, 0, 0.3 * args.cube_size_mpc)
    uniform_count = args.size * 2 // 5
    cluster_size, remainder = divmod(args.size - uniform_count, len(centers_x))
    cluster_counts = [cluster_size + int(index < remainder) for index in range(len(centers_x))]

    points = [uniform_point(rng, half_size) for _ in range(uniform_count)]
    for center_x, count in zip(centers_x, cluster_counts):
        points.extend(gaussian_point(rng, center_x, sigma, half_size) for _ in range(count))
    rng.shuffle(points)

    centers = ", ".join(f"{x:g}" for x in centers_x)
    counts = ", ".join(str(count) for count in cluster_counts)
    write_catalog(args, points, rng,
                  f"{uniform_count} uniform sources. Gaussian cluster counts are {counts} "
                  f"for centers x=({centers}) Mpc, y=z=0. "
                  f"Sigma is {sigma:g} Mpc per axis. Clusters are truncated to the cube.")


if __name__ == "__main__":
    main()
