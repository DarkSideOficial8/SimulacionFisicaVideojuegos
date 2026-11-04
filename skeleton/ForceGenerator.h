#pragma once
#include "Particle.h"
#include <cmath>

class ForceGenerator {
public:
    virtual ~ForceGenerator() = default;
    virtual void updateForce(Particle& particle, double dt) = 0;
};

class GravityForce : public ForceGenerator {
public:
    explicit GravityForce(Vector3D gravity) : gravity(gravity) {}
    void updateForce(Particle& p, double) override {
        p.addForce(gravity * p.getMass());
    }
private:
    Vector3D gravity;
};

class WindForce : public ForceGenerator {
public:
    WindForce(Vector3D windVelocity, Vector3D center, Vector3D halfExtents,
        float k1, float k2 = 0.f)
        : windVelocity(windVelocity), center(center), halfExtents(halfExtents),
        k1(k1), k2(k2) {}
    void updateForce(Particle& p, double dt) override;
protected:
    virtual Vector3D windAt(const Vector3D& position) const { return windVelocity; }
    bool inside(const Vector3D& position) const;
    Vector3D windVelocity, center, halfExtents;
    float k1, k2;
};

class VortexForce : public WindForce {
public:
    VortexForce(Vector3D center, float radius, float strength, float k1)
        : WindForce(Vector3D(), center, Vector3D(radius, radius, radius), k1),
        radius(radius), strength(strength) {}
    void updateForce(Particle& p, double dt) override;
protected:
    Vector3D windAt(const Vector3D& position) const override;
private:
    float radius, strength;
};

class ExplosionForce : public ForceGenerator {
public:
    ExplosionForce(Vector3D center, float radius, float strength, float tau)
        : center(center), radius(radius), strength(strength), tau(tau) {}
    void updateForce(Particle& p, double dt) override;
    void restart() { elapsed = 0.f; active = true; }
    void stop() { active = false; }
    void setCenter(const Vector3D& value) { center = value; }
    void advance(double dt) { if (active) { elapsed += static_cast<float>(dt); if (elapsed > 4.f * tau) active = false; } }
private:
    Vector3D center;
    float radius, strength, tau, elapsed = 0.f;
    bool active = false;
};
