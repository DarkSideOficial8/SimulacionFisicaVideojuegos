#pragma once
#include "Particle.h"
#include "ForceGenerator.h"
#include <cmath>

// El escalado conserva la energia cinetica inicial entre escala real y simulada.
class Projectile : public Particle {
public:
    enum class Type { Meteorite, ShockBall };
    Projectile(Type type, const Vector3D& origin, const Vector3D& direction,
        float realMass, float realSpeed, float simulationSpeed)
        : Particle(origin, direction* simulationSpeed,
            scaledMass(realMass, realSpeed, simulationSpeed), 1.f,
            15.f, type == Type::Meteorite ? 0.85f : 0.5f,
            type == Type::Meteorite ? Vector4(0.85f, 0.35f, 0.1f, 1.f)
            : Vector4(0.25f, 0.85f, 1.f, 1.f)),
        kind(type), realMassValue(realMass), realSpeedValue(realSpeed), simSpeedValue(simulationSpeed) {}

    static float scaledMass(float mass, float realSpeed, float simSpeed) {
        const float ratio = realSpeed / simSpeed;
        return mass * ratio * ratio;
    }
    float gravityAcceleration() const override { return scaledGravity(); }
    float scaledGravity(float realGravity = 9.81f) const {
        const float ratio = simSpeedValue / realSpeedValue;
        return realGravity * ratio * ratio;
    }
    // F_sim = m_sim * a_sim. Con este escalado equivale a F_real para gravedad.
    float scaledForce(float realForce) const { return realForce; }
    float realEnergy() const { return 0.5f * realMassValue * realSpeedValue * realSpeedValue; }
    float simulationEnergy() const { return 0.5f * getMass() * simSpeedValue * simSpeedValue; }
    Type type() const { return kind; }
private:
    Type kind;
    float realMassValue, realSpeedValue, simSpeedValue;
};

// La gravedad de un proyectil se escala para mantener la equivalencia energetica.
class ProjectileGravityForce : public ForceGenerator {
public:
    void updateForce(Particle& particle, double) override {
        particle.addForce(Vector3D(0.f, -particle.gravityAcceleration() * particle.getMass(), 0.f));
    }
};
