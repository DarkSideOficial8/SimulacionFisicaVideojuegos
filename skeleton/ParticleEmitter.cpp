#include "ParticleEmitter.h"
#include <algorithm>
#include <cmath>

ParticleEmitter::ParticleEmitter(Vector3D position, Vector3D meanVelocity, Vector3D variationValue,
    float particlesPerSecond, float lifetimeValue, float radiusValue, const Vector4& particleColor,
    Distribution distributionValue, Vector3D positionVariation, float dampingValue,
    Vector3D gravityValue)
    : origin(position), meanVel(meanVelocity), variation(variationValue),
    posVariation(positionVariation), gravity(gravityValue), rate(particlesPerSecond), lifetime(lifetimeValue),
    radius(radiusValue), damping(dampingValue), color(particleColor),
    distribution(distributionValue), rng(std::random_device{}()) {}

float ParticleEmitter::sample(float mean, float spread) {
    if (spread <= 0.0f) return mean;
    if (distribution == Distribution::Gaussian) {
        std::normal_distribution<float> dist(mean, spread); // spread = desviacion tipica
        return dist(rng);
    }
    std::uniform_real_distribution<float> dist(mean - spread, mean + spread);
    return dist(rng);
}

Vector3D ParticleEmitter::initialPosition() {
    return Vector3D(sample(origin.x, posVariation.x), sample(origin.y, posVariation.y),
        sample(origin.z, posVariation.z));
}
Vector3D ParticleEmitter::initialVelocity() {
    return Vector3D(sample(meanVel.x, variation.x), sample(meanVel.y, variation.y),
        sample(meanVel.z, variation.z));
}
bool ParticleEmitter::outside(const physx::PxVec3& p) const {
    return p.x < -45.f || p.x > 45.f || p.y < -15.f || p.y > 55.f ||
        p.z < -60.f || p.z > 35.f;
}
void ParticleEmitter::update(double dt) {
    if (dt <= 0.0) return;
    const float step = static_cast<float>(dt);
    for (auto& p : particles) {
        p->addForce(gravity * p->getMass());
        p->integrateEuler(dt);
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [this](const std::unique_ptr<Particle>& p) {
            return p->isDead() || outside(p->position());
        }), particles.end());

    accumulator += rate * step;
    const int births = static_cast<int>(accumulator);
    accumulator -= births;
    for (int i = 0; i < births && particles.size() < maxParticles; ++i) {
        particles.emplace_back(new Particle(initialPosition(), initialVelocity(),
            1.f, damping, lifetime, radius, color));
    }
}
void ParticleEmitter::clear() { particles.clear(); accumulator = 0.f; }
