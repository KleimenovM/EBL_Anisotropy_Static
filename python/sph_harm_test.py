import subprocess
import tempfile
from pathlib import Path

import numpy as np
from scipy.special import sph_harm
from config.settings import ROOT_DIR


L_MAX = 20
N_POINTS = 1000
ATOL = 1e-12
RTOL = 1e-12

cpp_path = ROOT_DIR / "src" / "geometry"
hpp_path = ROOT_DIR / "include" / "geometry"


def generate_points(n):
    rng = np.random.default_rng(12345)

    # theta: polar angle [0, pi]
    theta = np.arccos(rng.uniform(-1.0, 1.0, n))

    # phi: azimuthal angle [0, 2*pi)
    phi = rng.uniform(0.0, 2.0 * np.pi, n)

    return theta, phi


def build_cpp_driver():
    source = r"""
#include "SphericalHarmonics.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>

int main()
{
    constexpr int LMAX = LMAX_VALUE;
    constexpr int N = N_POINTS_VALUE;

    SphericalHarmonics sh(LMAX);

    std::cout << std::setprecision(17);

    double theta, phi;

    for (int i = 0; i < N; ++i)
    {
        std::cin >> theta >> phi;

        for (int l = 0; l <= LMAX; ++l)
        {
            for (int m = -l; m <= l; ++m)
            {
                const double y = sh.Ylm(l, m, std::cos(theta), phi);

                std::cout
                    << l << ' '
                    << m << ' '
                    << theta << ' '
                    << phi << ' '
                    << y << '\n';
            }
        }
    }

    return 0;
}
"""

    source = source.replace("LMAX_VALUE", str(L_MAX))
    source = source.replace("N_POINTS_VALUE", str(N_POINTS))

    path = Path(tempfile.mktemp(suffix=".cpp"))
    path.write_text(source)

    executable = path.with_suffix("")

    subprocess.run(
        [
            "g++",
            "-std=c++17",
            "-O2",
            str(path),
            str(cpp_path / "SphericalHarmonics.cpp"),
            "-I",
            str(hpp_path),
            "-o",
            str(executable),
        ],
        check=True,
        capture_output=False
    )
    
    return path, executable


def main():
    theta, phi = generate_points(N_POINTS)

    cpp_input = "".join(
        f"{t:.17g} {p:.17g}\n"
        for t, p in zip(theta, phi)
    )

    source, executable = build_cpp_driver()

    try:
        result = subprocess.run(
            [str(executable)],
            input=cpp_input,
            text=True,
            capture_output=True,
            check=True,
        )

        cpp_values = {}

        for line in result.stdout.splitlines():
            l, m, t, p, value = line.split()

            key = (int(l), int(m), float(t), float(p))
            cpp_values[key] = float(value)

        max_abs_error = 0.0
        max_rel_error = 0.0
        worst_case = None

        for i, (t, p) in enumerate(zip(theta, phi)):
            for l in range(L_MAX + 1):
                for m in range(-l, l + 1):
                    cpp_value = cpp_values[(l, m, float(t), float(p))]

                    # scipy uses theta = polar angle, phi = azimuth.
                    python_value = sph_harm(m, l, p, t)

                    # The C++ implementation is expected to return real-valued
                    if m > 0:
                        reference = np.sqrt(2) * python_value.real
                    elif m < 0:
                        reference = (-1)**(m+1) * np.sqrt(2) * python_value.imag
                    else:
                        reference = python_value.real

                    abs_error = abs(cpp_value - reference)

                    scale = max(abs(reference), abs(cpp_value), 1e-300)
                    rel_error = abs_error / scale

                    if abs_error > max_abs_error:
                        max_abs_error = abs_error
                        worst_case = (
                            i, l, m, t, p,
                            cpp_value, reference,
                        )

                    max_rel_error = max(max_rel_error, rel_error)

                    if not np.isclose(
                        cpp_value,
                        reference,
                        atol=ATOL,
                        rtol=RTOL,
                    ):
                        print("FAIL")
                        print(f"l = {l}, m = {m}")
                        print(f"theta = {t}")
                        print(f"phi = {p}")
                        print(f"C++      = {cpp_value:.17e}")
                        print(f"Python   = {reference:.17e}")
                        print(f"abs err  = {abs_error:.3e}")
                        print(f"rel err  = {rel_error:.3e}")
                        return 1

        print("PASS")
        print(f"L_max              = {L_MAX}")
        print(f"Points              = {N_POINTS}")
        print(f"Maximum abs. error  = {max_abs_error:.3e}")
        print(f"Maximum rel. error  = {max_rel_error:.3e}")

        if worst_case is not None:
            _, l, m, t, p, cpp_value, reference = worst_case
            print()
            print("Worst case:")
            print(f"  l        = {l}")
            print(f"  m        = {m}")
            print(f"  theta    = {t:.17g}")
            print(f"  phi      = {p:.17g}")
            print(f"  C++      = {cpp_value:.17e}")
            print(f"  Python   = {reference:.17e}")

        return 0

    finally:
        source.unlink(missing_ok=True)
        executable.unlink(missing_ok=True)


if __name__ == "__main__":
    raise SystemExit(main())
