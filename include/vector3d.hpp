#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <cmath>
#include <iostream>


class Vector3D
{
public:

    double x;
    double y;
    double z;

    Vector3D();

    Vector3D(
        double x_,
        double y_,
        double z_
    );

    double norm() const;
    double norm2() const;
    double dot(const Vector3D& other) const;

    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D& operator+=(const Vector3D& other);
    
    Vector3D operator*(double scalar) const;
    Vector3D operator/(double scalar) const;

    friend std::ostream& operator<<(
        std::ostream& os,
        const Vector3D& v
    );

};

#endif