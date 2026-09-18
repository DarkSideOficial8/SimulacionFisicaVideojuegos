#include "Vector3D.h"

Vector3D::Vector3D(float nx, float ny, float nz)
{
	x = nx;
	y = ny;
	z = nz;
}
Vector3D::Vector3D(physx::PxVec3 v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

float Vector3D::magnitude() const
{
	float sol = std::sqrt(x * x + y * y + z * z);
	return sol;
}

Vector3D Vector3D::normalize() const
{
	Vector3D sol;
	float mag = magnitude();

	sol.x = x / mag;
	sol.y = y / mag;
	sol.z = z / mag;

	return sol;
}

float Vector3D::dot(const Vector3D& v) const
{
	float sol = x * v.x + y * v.y + z * v.z;
	return sol;
}

Vector3D Vector3D::cross(const Vector3D& v) const
{
	Vector3D sol;

	sol.x = (y * v.z - z * v.y);
	sol.y = (x * v.z - z * v.x);
	sol.z = (x * v.y - y * v.x);

	return sol;
}

Vector3D& Vector3D::operator=(const Vector3D& v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}

Vector3D Vector3D::operator+(const Vector3D& v) const
{
	return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D Vector3D::operator-(const Vector3D& v) const
{
	return Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D Vector3D::operator*(float escalar) const
{
	return Vector3D(x * escalar, y * escalar, z * escalar);
}

Vector3D& Vector3D::operator+=(const Vector3D& v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

Vector3D::operator physx::PxVec3() const
{
	return physx::PxVec3(x, y, z);
}