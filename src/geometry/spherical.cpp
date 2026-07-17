#include "geometry/spherical.hpp"

#include <cmath>

namespace
{
    // Rotation matrix: Equatorial (J2000) -> Galactic
    constexpr float EQ_TO_GAL[3][3] =
    {
        {-0.0548755604f, -0.8734370902f, -0.4838350155f},
        { 0.4941094279f, -0.4448296300f,  0.7469822445f},
        {-0.8676661490f, -0.1980763734f,  0.4559837762f}
    };

    constexpr float TWO_PI = 2.0f * static_cast<float>(M_PI);

    float normalizeAngle(float angle)
    {
        while (angle < 0.0f)
            angle += TWO_PI;

        while (angle >= TWO_PI)
            angle -= TWO_PI;

        return angle;
    }
}


// Equatorial <-> Galactic

Galactic equatorialToGalactic(const Equatorial& eq)
{
    const float cos_dec = std::cos(eq.dec);

    const float x = cos_dec * std::cos(eq.ra);
    const float y = cos_dec * std::sin(eq.ra);
    const float z = std::sin(eq.dec);

    // Forward transformation
    const float gx =
        EQ_TO_GAL[0][0] * x +
        EQ_TO_GAL[0][1] * y +
        EQ_TO_GAL[0][2] * z;

    const float gy =
        EQ_TO_GAL[1][0] * x +
        EQ_TO_GAL[1][1] * y +
        EQ_TO_GAL[1][2] * z;

    const float gz =
        EQ_TO_GAL[2][0] * x +
        EQ_TO_GAL[2][1] * y +
        EQ_TO_GAL[2][2] * z;

    return {
        normalizeAngle(std::atan2(gy, gx)),
        std::asin(gz),
        eq.distance
    };
}


Equatorial galacticToEquatorial(const Galactic& gal)
{
    const float cos_lat = std::cos(gal.lat);

    const float gx = cos_lat * std::cos(gal.lon);
    const float gy = cos_lat * std::sin(gal.lon);
    const float gz = std::sin(gal.lat);

    // Inverse transformation (=transpose) 
    const float x =
        EQ_TO_GAL[0][0] * gx +
        EQ_TO_GAL[1][0] * gy +
        EQ_TO_GAL[2][0] * gz;

    const float y =
        EQ_TO_GAL[0][1] * gx +
        EQ_TO_GAL[1][1] * gy +
        EQ_TO_GAL[2][1] * gz;

    const float z =
        EQ_TO_GAL[0][2] * gx +
        EQ_TO_GAL[1][2] * gy +
        EQ_TO_GAL[2][2] * gz;

    return {
        normalizeAngle(std::atan2(y, x)),
        std::asin(z),
        gal.distance
    };
}


// Galactic -> Cartesian (Galactic)
Vector3D toCartesian(const Galactic& gal)
{
    const float cos_lat = std::cos(gal.lat);

    return Vector3D(
        gal.distance * cos_lat * std::cos(gal.lon),
        gal.distance * cos_lat * std::sin(gal.lon),
        gal.distance * std::sin(gal.lat)
    );
}

// Equatorial -> Cartesian (Galactic)
Vector3D toCartesian(const Equatorial& eq)
{
    auto gal = equatorialToGalactic(eq);

    return toCartesian(gal);
}

// Cartesian (Galactic) -> Galactic
Galactic toGalactic(const Vector3D& pos)
{
    const float r = pos.norm();

    return {
        normalizeAngle(std::atan2(pos.y, pos.x)),
        std::asin(pos.z / r),
        r
    };
}

// Cartesian (Galactic) -> Equatorial
Equatorial toEquatorial(const Vector3D& pos)
{
    auto gal = toGalactic(pos);

    return galacticToEquatorial(gal);
}