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
    double cos_theta,
    double phi
) const
{
    assert(l >= 0);
    assert(l <= lmax_);
    assert(std::abs(m) <= l);

    int abs_m = std::abs(m);

    const double P = associatedLegendre(l, abs_m, cos_theta);

    const double N = normalization_[normalizationIndex(l, abs_m)];

    if (m > 0)
        return SQRT2 * N * P * std::cos(abs_m * phi);

    if (m < 0)
        return SQRT2 * N * P * std::sin(abs_m * phi);

    return N * P;
}

std::vector<double> SphericalHarmonics::FullYlm(
    int l_max,
    double cos_theta,
    double sin_phi,
    double cos_phi
) const 
{
    assert(l_max >= 0);
    assert(l_max <= lmax_);  // to remain within precomputed normalization boundaries

    std::vector<double> sin_m_phi(2*l_max + 1, 0.0);
    std::vector<double> cos_m_phi(2*l_max + 1, 1.0);
    std::vector<double> result((l_max + 1) * (l_max + 1), normalization_[0]);

    if (l_max == 0)
        // The case of l_max = 0 is special: sin_m_phi[1] is ill defined
        return result;

    sin_m_phi[1] = sin_phi;
    cos_m_phi[1] = cos_phi;

    // precompute multiple angles based on sin & cos values only
    for (int m_abs = 2; m_abs < l_max + 1; m_abs++){
        sin_m_phi[m_abs] = sin_m_phi[m_abs-1] * cos_phi + cos_m_phi[m_abs-1] * sin_phi;
        cos_m_phi[m_abs] = cos_m_phi[m_abs-1] * cos_phi - sin_m_phi[m_abs-1] * sin_phi;
    }

    // fill the final vector by spherical harmonics
    for (int l = 0; l < l_max + 1; l++){
        for (int m = -l; m < l + 1; m++){

            int abs_m = std::abs(m);

            const double P = associatedLegendre(l, abs_m, cos_theta);
            const double N = normalization_[normalizationIndex(l, abs_m)];
            
            int idx = index(l, m);  // l(l+1) + m

            if (m > 0){
                result[idx] = (SQRT2 * N * P * cos_m_phi[abs_m]);
            } else if (m < 0){
                result[idx] = (SQRT2 * N * P * sin_m_phi[abs_m]);
            } else {
                result[idx] = N*P;
            }
        }
    }

    return result;
}
