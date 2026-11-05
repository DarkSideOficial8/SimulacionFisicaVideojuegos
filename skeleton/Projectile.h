#pragma once
#include "Particle.h"
#include <cmath>

// Proyectil con escalado energetico (Practica 1).
class Projectile : public Particle {
public:
    enum class Type { Sphere, Box };
    Projectile(Type type, const Vector3D& position, const Vector3D& direction,
               float realMass, float realSpeed, float simSpeed)
        : Particle(position, direction * simSpeed,
                   scaledMass(realMass, realSpeed, simSpeed), 1.f,
                   100.f, 0.2f,
                   type == Type::Sphere ? Vector4(1.f, 0.25f, 0.1f, 1.f)
                                        : Vector4(0.2f, 0.8f, 1.f, 1.f), false),
          kind(type), simGravity(scaledGravity(realSpeed, simSpeed)),
          shapePose(physx::PxTransform(position)) {
        // El RenderItem del proyectil usa la geometria adecuada.
        const auto sphere = physx::PxSphereGeometry(0.2f);
        const auto box = physx::PxBoxGeometry(0.2f, 0.2f, 0.2f);
        shape = new RenderItem(CreateShape(type == Type::Sphere
            ? static_cast<const physx::PxGeometry&>(sphere)
            : static_cast<const physx::PxGeometry&>(box)),
            &shapePose, type == Type::Sphere ? Vector4(1.f, 0.25f, 0.1f, 1.f)
                                             : Vector4(0.2f, 0.8f, 1.f, 1.f));
    }
    ~Projectile() override { if (shape) shape->release(); }
    void syncRender() { shapePose.p = position(); }
    float gravity() const { return simGravity; }
    Type type() const { return kind; }
    static float scaledMass(float m, float realSpeed, float simSpeed) {
        const float ratio = realSpeed / simSpeed;
        return m * ratio * ratio;
    }
    static float scaledGravity(float realSpeed, float simSpeed) {
        const float ratio = simSpeed / realSpeed;
        return 9.81f * ratio * ratio;
    }
    static float realEnergy(float m, float speed) { return 0.5f * m * speed * speed; }
    static float simulationEnergy(float m, float realSpeed, float simSpeed) {
        return 0.5f * scaledMass(m, realSpeed, simSpeed) * simSpeed * simSpeed;
    }
private:
    Type kind;
    float simGravity;
    physx::PxTransform shapePose;
    RenderItem* shape = nullptr;
};
