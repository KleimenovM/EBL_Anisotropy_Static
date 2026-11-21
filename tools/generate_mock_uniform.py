"""Generate a uniformly distributed mock octree catalog."""

import argparse
import math
from pathlib import Path
from random import Random


DATA_DIR = Path(__file__).resolve().parents[1] / "data"
PC_PER_MPC = 1_000_000


def parse_catalog_args(description, filename):
    parser = argparse.ArgumentParser(description=description,
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--size", type=int, default=400, help="number of sources")
    scale = parser.add_mutually_exclusive_group()
    scale.add_argument("--cube-size-mpc", type=float, default=500, help="cube side length in Mpc")
    scale.add_argument("--cube-size-pc", type=float, help="cube side length in pc")
    parser.add_argument("--seed", type=int, default=20261120, help="random seed")
    parser.add_argument("--output", type=Path, default=DATA_DIR / filename, help="catalog path")
    args = parser.parse_args()
    if args.size < 1:
        parser.error("--size must be positive")
    if args.cube_size_pc is not None:
        args.cube_size_mpc = args.cube_size_pc / PC_PER_MPC
    if not math.isfinite(args.cube_size_mpc) or args.cube_size_mpc <= 0:
        parser.error("cube side length must be finite and positive")
    return args


def uniform_point(rng, half_size):
    return tuple(rng.uniform(-half_size, half_size) for _ in range(3))


def write_catalog(args, points, rng, description):
    with args.output.open("w", encoding="ascii", newline="\n") as output:
        output.write("# x y z mass\n")
        output.write(f"# Coordinates are in Mpc. Cube side is {args.cube_size_mpc:g} Mpc. "
                     f"RNG seed is {args.seed}.\n")
        output.write("# Masses are drawn from an exponential distribution with mean 1.\n")
        output.write(f"# {description}\n")
        for point in points:
            coordinates = " ".join(f"{value:.12e}" for value in point)
            mass = rng.expovariate(1.0)
            output.write(f"{coordinates} {mass:.12e}\n")


def main():
    args = parse_catalog_args(__doc__, "mock_uniform.txt")
    rng = Random(args.seed)
    half_size = args.cube_size_mpc / 2
    points = [uniform_point(rng, half_size) for _ in range(args.size)]
    write_catalog(args, points, rng, f"{args.size} uniformly distributed sources.")


if __name__ == "__main__":
    main()
