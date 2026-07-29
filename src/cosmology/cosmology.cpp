#include "cosmology/cosmology.hpp"

#include <cmath>
#include <stdexcept>
#include <algorithm>


// Hubble constant in flat FLRW metric with matter and cosmological constant
static double H(double a, double H0, double omega_m)
{
    double omega_lambda = 1.0 - omega_m;
    double a3 = a * a * a;
    return H0 * std::sqrt(omega_m / a3 + omega_lambda);
}

double Cosmology::dtau_dloga(double a)
{
    return 1.0 / (a * H(a, H0, omega_m));
}

double Cosmology::dt_dloga(double a)
{
    return 1.0 / H(a, H0, omega_m);
}


Cosmology::Cosmology(
    double H0,
    double omega_m,
    std::size_t n_points
):  
    H0(H0), omega_m(omega_m),
    a_of_tau_({0.0, 1.0}, {0.0, 1.0}),
    a_of_lumdist_({0.0, 1.0}, {0.0, 1.0}),
    tau_of_loga_({0.0, 1.0}, {0.0, 1.0}),
    t_of_tau_({0.0, 1.0}, {0.0, 1.0})
{
    const double a_0 = 1.0;                 // today's scale
    const double z_max = 20.0;              // maximal considered redshift
    const double a_min = a_0 / (1 + z_max); // minimal scale value

    std::vector<double> tau(n_points);      // conformal time ( = -comovong distance d_c)
    std::vector<double> lumdist(n_points);  // luminosity distance, d_L = d_c/a
    std::vector<double> t(n_points);        // cosmic time t
    std::vector<double> a(n_points);        // scale parameter a
    std::vector<double> loga(n_points);     // scale logarithm log(a)

    // Choose a uniform grid in log(a)
    double log_a_min = std::log(a_min);
    double log_a_max = std::log(a_0);                           // equal to 0
    double dlog_a = (log_a_max - log_a_min) / (n_points - 1);   // positive

    for (int i = 0; i < n_points; ++i)
    {   
        auto loga_i = log_a_max - i * dlog_a;
        loga[i] = loga_i;
        a[i] = std::exp(loga_i);
    }

    // Integrate by using the trapezoidal rule in the log-axis

    tau[0] = 0.0;
    lumdist[0] = 0.0;
    t[0] = 0.0;

    for (int i = 1; i < n_points; ++i)
    {
        tau[i] = tau[i - 1] - 0.5 * dlog_a * (dtau_dloga(a[i - 1]) + dtau_dloga(a[i]));
        lumdist[i] = tau[i] / a[i];
        t[i] = t[i - 1] - 0.5 * dlog_a * (dt_dloga(a[i - 1]) + dt_dloga(a[i]));
    }

    // Reverse the arrays to have them sorted in increasing order

    std::reverse(tau.begin(), tau.end());
    std::reverse(lumdist.begin(), lumdist.end());
    std::reverse(t.begin(), t.end());
    std::reverse(a.begin(), a.end());
    std::reverse(loga.begin(), loga.end());

    
    // Store results as lookup tables
    a_of_tau_ = Lookup(tau, a);
    a_of_lumdist_ = Lookup(lumdist, a);
    tau_of_loga_ = Lookup(loga, tau);
    t_of_tau_ = Lookup(tau, t);
}


double Cosmology::tau2a(double tau) const
{
    return a_of_tau_(tau);
}

double Cosmology::a2tau(double a) const
{
    double loga = std::log(a);
    return tau_of_loga_(loga);
}

double Cosmology::tau2t(double tau) const
{
    return t_of_tau_(tau);
}

double Cosmology::dl2tau(double dl) const
{
    double loga_from_dl = std::log(a_of_lumdist_(-dl));
    return tau_of_loga_(loga_from_dl);
}

double Cosmology::tau2dl(double tau) const
{
    double a_tau = a_of_tau_(tau);
    return -tau / a_tau;
}


const Cosmology& standardFlatCosmology()
{
    constexpr double H0 = 70.0;      // [km s-1 Mpc-1]
    constexpr double c = 3.0e5;      // [km s-1]
    constexpr double H0c = H0 / c;   // [Mpc-1]
    constexpr double omega_m = 0.3;

    static const Cosmology cosmology(H0c, omega_m);

    return cosmology;
}
