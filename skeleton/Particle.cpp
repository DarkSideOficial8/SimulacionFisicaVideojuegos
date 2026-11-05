#include "Particle.h"
#include "ForceGenerator.h"
#include <cmath>

Particle::Particle(Vector3D pos, Vector3D v, float m, float d, const physx::PxGeometry& geometry)
    : vel(v), prevPos(pos), mass(m > 0.f ? m : 1.f), damping(d), lifetime(1000000.f), pose(physx::PxTransform(pos)) {
    renderItem = new RenderItem(CreateShape(geometry), &pose, Vector4(1.f, 0.f, 0.f, 1.f));
}
Particle::Particle(Vector3D pos, Vector3D v, float m, float d, float life, float radiusValue, Vector4 colorValue, bool visible)
    : vel(v), prevPos(pos), mass(m > 0.f ? m : 1.f), damping(d), lifetime(life), radius(radiusValue), color(colorValue), pose(physx::PxTransform(pos)) {
    if (visible) renderItem = new RenderItem(CreateShape(physx::PxSphereGeometry(radiusValue)), &pose, colorValue);
}

Particle::~Particle() {
    if (renderItem)
        renderItem->release();
}

void Particle::addForce(const Vector3D& force) {
    forceAccum += force;
}
void Particle::applyForceGenerator(ForceGenerator& generator, double dt) {
    generator.updateForce(*this, dt);
}

void Particle::clearForce() {
    forceAccum = Vector3D();
}

void Particle::integrateEuler(double dt) {
    if (dt <= 0.0 || isDead()) {
        clearForce(); return;
    }
    float step = static_cast<float>(dt);

    Vector3D acc = forceAccum * (1.f / mass);
    vel = (vel + acc * step) * std::pow(damping, step);

    pose.p += static_cast<physx::PxVec3>(vel * step);
    elapsed += step;

    clearForce();
}
void Particle::integrateVerlet(double dt) {
    if (dt <= 0.0 || isDead()) {
        clearForce();
        return;
    }

    float step = static_cast<float>(dt);

    Vector3D current = pose.p;
    Vector3D acc = forceAccum * (1.f / mass);
    Vector3D next = current * 2.f - prevPos + acc * (step * step);

    vel = (next - prevPos) * (1.f / (2.f * step));
    pose.p = static_cast<physx::PxVec3>(next);
    prevPos = current;
    elapsed += step;

    clearForce();
}
