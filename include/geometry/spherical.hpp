#include "geometry/vector3d.hpp"


struct Equatorial
{
    float ra;        // rad
    float dec;       // rad
    float distance;  // Mpc
};

struct Galactic
{
    float lon;       // rad
    float lat;       // rad
    float distance;  // Mpc
};


Vector3D toCartesian(const Equatorial&);
Vector3D toCartesian(const Galactic&);

Equatorial toEquatorial(const Vector3D&);
Galactic   toGalactic(const Vector3D&);

Galactic   equatorialToGalactic(const Equatorial&);
Equatorial galacticToEquatorial(const Galactic&);
