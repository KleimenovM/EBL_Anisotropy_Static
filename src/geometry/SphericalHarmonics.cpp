#include "SphericalHarmonics.hpp"

#include <cassert>
#include <cmath>


namespace
{
    constexpr double FOUR_PI = 4.0 * 3.141592653589793238463;
    constexpr double SQRT2 = 1.41421356237309504880;

    double factorial(int n)
    {
        double result = 1.0;

        for (int i = 2; i <= n; ++i)
            result *= i;

        return result;
    }
}

SphericalHarmonics::SphericalHarmonics(int lmax)
    : lmax_(lmax)
{
    normalization_.resize((lmax + 1) * (lmax + 2) / 2);

    for (int l = 0; l <= lmax; ++l)
    {
        for (int m = 0; m <= l; ++m)
        {
            normalization_[normalizationIndex(l, m)] =
                std::sqrt((2.0 * l + 1.0) * factorial(l - m) / (FOUR_PI * factorial(l + m))
                );
        }
    }
}

double SphericalHarmonics::associatedLegendre(
    int l,
    int m,
    double x
) const
{
    assert(m >= 0);

    double pmm = 1.0;

    if (m > 0)
    {
        double somx2 = std::sqrt((1.0 - x) * (1.0 + x));
        double fact = 1.0;

        for (int i = 1; i <= m; ++i)
        {
            pmm *= -fact * somx2;
            fact += 2.0;
        }
    }

    if (l == m)
        return pmm;

    double pmmp1 = (2 * m + 1) * x * pmm;

    if (l == m + 1)
        return pmmp1;

    double pll = 0.0;

    for (int ll = m + 2; ll <= l; ++ll)
    {
        pll = ((2 * ll - 1) * x * pmmp1 - (ll + m - 1) * pmm) / (ll - m);

        pmm = pmmp1;
        pmmp1 = pll;
    }

    return pll;
}

double SphericalHarmonics::Ylm(
    int l,
    int m,
    double theta,
    double phi
) const
{
    assert(l >= 0);
    assert(l <= lmax_);
    assert(std::abs(m) <= l);

    int abs_m = std::abs(m);

    const double P = associatedLegendre(l, abs_m, std::cos(theta));

    const double N = normalization_[normalizationIndex(l, abs_m)];

    if (m > 0)
        return SQRT2 * N * P * std::cos(abs_m * phi);

    if (m < 0)
        return SQRT2 * N * P * std::sin(abs_m * phi);

    return N * P;
}