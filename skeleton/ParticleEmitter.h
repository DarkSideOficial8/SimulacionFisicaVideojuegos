#pragma once
#include "Particle.h"
#include <vector>
#include <memory>
#include <random>
#include <cstddef>

class ParticleEmitter {
public:
    enum class Distribution { Uniform, Gaussian };
    ParticleEmitter(Vector3D position, Vector3D meanVelocity, Vector3D variation,
        float particlesPerSecond, float lifetime, float radius,
        const Vector4& color, Distribution distribution,
        Vector3D positionVariation = Vector3D(), float damping = 1.0f,
        Vector3D gravity = Vector3D(0.f, -10.f, 0.f));
    void update(double dt);
    void clear();
    std::size_t count() const { return particles.size(); }
private:
    float sample(float mean, float variation);
    Vector3D initialPosition();
    Vector3D initialVelocity();
    bool outside(const physx::PxVec3& p) const;
    Vector3D origin, meanVel, variation, posVariation, gravity;
    float rate, lifetime, radius, damping, accumulator = 0.0f;
    Vector4 color;
    Distribution distribution;
    std::mt19937 rng;
    std::vector<std::unique_ptr<Particle>> particles;
    static constexpr std::size_t maxParticles = 2500;
};
