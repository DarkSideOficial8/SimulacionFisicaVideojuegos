#pragma once

#include <string>
#include <cmath>
#include "PxPhysicsAPI.h"

class Vector3D {
public:
	float x, y, z;

	Vector3D(float x = 0, float y = 0, float z = 0);
	Vector3D(physx::PxVec3 v);

	float magnitude() const;
	Vector3D normalize() const;

	float dot(const Vector3D& v) const;
	Vector3D cross(const Vector3D& v) const;

	Vector3D& operator=(const Vector3D& v);
	Vector3D operator+(const Vector3D& v) const;
	Vector3D operator-(const Vector3D& v) const;
	Vector3D operator*(float escalar) const;
	Vector3D& operator+=(const Vector3D& v);

	operator physx::PxVec3() const;


};