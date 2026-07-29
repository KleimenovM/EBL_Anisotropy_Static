#include "cosmology/cosmology.hpp"

#include <iostream>
#include <cmath>


double a_exact(double t, double H0, double omega_m)
{
    double omega_lambda = 1.0 - omega_m;

    // today relative to the Big
    double t0 = 2/(3 * std::sqrt(omega_lambda) * H0) * std::asinh(std::sqrt(omega_lambda / omega_m));

    // in our calculations, today is t = 0. Shift it by Big Bang time
    double x = 1.5 * H0 * std::sqrt(omega_lambda) * (t0 + t);

    return std::pow(omega_m / omega_lambda, 1.0 / 3.0) * std::pow(std::sinh(x), 2.0 / 3.0);
}


int main(){
    double H0 = 70;  // [km s-1 Mpc-1]
    double c = 3e5;  // [km s-1]
    double H0c = H0 / c;  // [Mpc-1]
    double omega_m = 0.3;

    auto tolerance = 1e-4;

    auto cosmo = Cosmology(H0c, omega_m);

    int break_counter = 0;

    // check a -> tau -> a
    double a = 0.05;
    while (a <= 1.0)
    {
        double tau = cosmo.a2tau(a);
        double a2 = cosmo.tau2a(tau);

        if (std::abs(a2/a - 1) > tolerance)
        {
            std::cout << "ERROR, a -> tau -> a procedure failed." << std::endl;
            std::cout << a2 << " to " << tau << " back to " << a << std::endl;
            break_counter ++;
            break;
        }

        a += 0.005;
    }
    if (break_counter == 0)
    {
        std::cout << "SUCCESS. a -> tau -> a" << std::endl;
        break_counter = 0;
    }


    // check tau -> a -> tau
    for (double tau = -2e3; tau <= 0.0; tau += 5.0)
    {
        double a = cosmo.tau2a(tau);
        double tau2 = cosmo.a2tau(a);
        
        if (std::abs(tau2/tau - 1) > tolerance)
        {
            std::cout << "ERROR, tau -> a -> tau procedure failed." << std::endl;
            std::cout << tau2 << " from " << tau << std::endl;
            break_counter ++;
            break;
        }

    }
    if (break_counter == 0)
    {
        std::cout << "SUCCESS. tau -> a -> tau" << std::endl;
        break_counter = 0;
    }


    // check tau -> d_L -> tau
    for (double tau = -2e3; tau <= 0.0; tau += 5.0)
    {
        double dl = cosmo.tau2dl(tau);
        double tau2 = cosmo.dl2tau(dl);
        
        if (std::abs(tau2/tau - 1) > tolerance)
        {
            std::cout << "ERROR, tau -> d_l -> tau procedure failed." << std::endl;
            std::cout << tau2 << " to " << dl <<" back to " << tau << std::endl;
            break_counter ++;
            break;
        }
    }
    if (break_counter == 0)
    {
        std::cout << "SUCCESS. tau -> d_L -> tau" << std::endl;
        break_counter = 0;
    }


    // check low redshifts (d_L ~ z / H0c)
    double z_low = 1e-6;
    double a_low = 1 / (1 + z_low);
    double dl_low = cosmo.tau2dl(cosmo.a2tau(a_low));

    if (std::abs(dl_low / (z_low / H0c) - 1) > tolerance)
    {
        std::cout << "ERROR, tau -> d_l -> low redshift procedure failed." << std::endl;
        std::cout << dl_low << " instead of " << z_low / H0c << std::endl;
    }
    else {
        std::cout << "SUCCESS. Low redshift test" << std::endl;
    }


    // check analytical solutions
    for (double tau = -2e3; tau <= 0.0; tau += 5.0)
    {
        double t = cosmo.tau2t(tau);
        double a = cosmo.tau2a(tau);
        double a_analyt = a_exact(t, H0c, omega_m);
        
        if (std::abs(a / a_analyt - 1) > tolerance)
        {
            std::cout << "ERROR, Exact solution cross-check failed." << std::endl;
            std::cout << a << " instead of " << a_analyt <<" at tau = " << tau << " or t = " << t << std::endl;
            break_counter ++;
            break;
        }
    }
    if (break_counter == 0)
    {
        std::cout << "SUCCESS. Exact solution cross-check" << std::endl;
        break_counter = 0;
    }

    return 0;
}