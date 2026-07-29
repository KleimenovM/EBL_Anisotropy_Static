#pragma once

#include <vector>
#include "lookup.hpp"

class Cosmology
{
public:
    Cosmology(
        double H0,
        double omega_m,
        std::size_t n_points = 10000
    );
    const double H0;
    const double omega_m;

    // conformal time ( = comoving distance) <-> scale factor and cosmic time 
    double tau2a(double tau) const;
    double a2tau(double a) const;
    double tau2t(double tau) const;

    // luminosity distance <-> conformal time ( = comoving distance)
    double dl2tau(double dl) const;
    double tau2dl(double tau) const;

private:
    Lookup a_of_tau_;
    Lookup a_of_lumdist_;
    Lookup tau_of_loga_;
    Lookup t_of_tau_;

    double dtau_dloga(double a);
    double dt_dloga(double a);
};


const Cosmology& standardFlatCosmology();