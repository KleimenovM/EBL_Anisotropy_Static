#pragma once

#include <vector>

class SphericalHarmonics
{
public:

    explicit SphericalHarmonics(int lmax);

    int lmax() const
    {
        return lmax_;
    }

    int numberOfCoefficients() const
    {
        return (lmax_ + 1) * (lmax_ + 1);
    }

    static int index(int l, int m)
    {
        return l * (l + 1) + m;
    }

    double Ylm(
        int l,
        int m,
        double cos_theta,
        double phi
    ) const;

    std::vector<double> FullYlm(
        int l_max,
        double cos_theta,
        double sin_phi,
        double cos_phi
    ) const;

private:

    double associatedLegendre(
        int l,
        int m,
        double x
    ) const;

    int normalizationIndex(
        int l,
        int m
    ) const
    {
        return l * (l + 1) / 2 + m;
    }

    int lmax_;

    // Stores N(l,m), m>=0
    std::vector<double> normalization_;
};