#include "geometry/vector3d.hpp"


Vector3D::Vector3D()
    :
    x(0.0),
    y(0.0),
    z(0.0)
{
}


Vector3D::Vector3D(
    float x_,
    float y_,
    float z_
)
    :
    x(x_),
    y(y_),
    z(z_)
{
}

float Vector3D::norm2() const
{
    return x*x + y*y + z*z;
}

float Vector3D::norm() const
{
    return std::sqrt(norm2());
}

float Vector3D::dot(const Vector3D& other) const
{
    return x*other.x
         + y*other.y
         + z*other.z;
}


Vector3D Vector3D::operator+(const Vector3D& other) const
{
    return Vector3D(
        x + other.x,
        y + other.y,
        z + other.z
    );
}

Vector3D Vector3D::operator-( const Vector3D& other) const
{
    return Vector3D(
        x - other.x,
        y - other.y,
        z - other.z
    );
}

Vector3D& Vector3D::operator+=(const Vector3D& other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Vector3D Vector3D::operator*(float scalar) const
{
    return Vector3D(
        scalar*x,
        scalar*y,
        scalar*z
    );
}

Vector3D Vector3D::operator/(float scalar) const
{
    return Vector3D(
        x / scalar,
        y / scalar,
        z / scalar
    );
}

std::ostream& operator<<(std::ostream& os, const Vector3D& v)
{
    os << "("
       << v.x << ", "
       << v.y << ", "
       << v.z
       << ")";

    return os;
}

