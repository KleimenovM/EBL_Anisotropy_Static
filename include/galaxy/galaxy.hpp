#include "geometry/vector3d.hpp"


struct Galaxy
{
    Vector3D position;   // Cartesian (Galactic) coordinates (Mpc)

    float logMstar;      // log10(M*/Msun)
    float logSFR;        // log10(Msun/yr)
};

