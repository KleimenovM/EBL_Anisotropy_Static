#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <cmath>
#include <iostream>


class Vector3D
{
public:

    float x;
    float y;
    float z;

    Vector3D();

    Vector3D(
        float x_,
        float y_,
        float z_
    );

    float norm() const;
    float norm2() const;
    float dot(const Vector3D& other) const;

    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D& operator+=(const Vector3D& other);
    
    Vector3D operator*(float scalar) const;
    Vector3D operator/(float scalar) const;

    friend std::ostream& operator<<(
        std::ostream& os,
        const Vector3D& v
    );

};

#endif