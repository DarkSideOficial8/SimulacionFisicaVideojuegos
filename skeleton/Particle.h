#pragma once
#include "PxPhysicsAPI.h"
#include "Vector3D.h"
#include "RenderUtils.hpp"

class ForceGenerator;

class ForceGenerator;

class Particle {
public:
    Particle(Vector3D pos, Vector3D vel, float mass, float damping, const physx::PxGeometry& geometry);
    Particle(Vector3D pos, Vector3D vel, float mass, float damping, float lifetime, float radius, Vector4 color, bool visible = true);
    virtual ~Particle();
    Particle(const Particle&) = delete;
    Particle& operator=(const Particle&) = delete;

    void addForce(const Vector3D& force);
    void clearForce();
    void applyForceGenerator(ForceGenerator& generator, double dt);

    void integrateEuler(double dt);
    void integrateVerlet(double dt);

    bool isDead() const { return elapsed >= lifetime; }

    const physx::PxVec3& position() const { return pose.p; }
    Vector3D velocity() const { return vel; }

    float getMass() const { return mass; }
    float age() const { return elapsed; }
    float getDamping() const { return damping; }
    float getLifetime() const { return lifetime; }
    float getRadius() const { return radius; }
    Vector4 getColor() const { return color; }

private:
    Vector3D vel, prevPos, forceAccum;
    float mass, damping, lifetime, elapsed = 0.f;
    float radius = 0.2f;
    Vector4 color = Vector4(1.f, 0.f, 0.f, 1.f);
    physx::PxTransform pose;
    RenderItem* renderItem = nullptr;
};
