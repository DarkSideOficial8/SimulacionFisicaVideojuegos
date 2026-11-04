#pragma once
#include "Particle.h"
#include <memory>
#include <random>
#include <vector>

// Emisor que guarda una particula modelo y genera copias de sus propiedades.
class ParticleGenerator {
public:
    enum class Distribution { Uniform, Gaussian };
    ParticleGenerator(const Vector3D& position, const Vector3D& velocity,
        const Vector3D& positionSpread, const Vector3D& velocitySpread,
        float rate, Distribution distribution, float mass, float damping,
        float life, float radius, const Vector4& color)
        : prototype(new Particle(position, velocity, mass, damping, life, radius, color, false)),
          origin(position), meanVelocity(velocity), posSpread(positionSpread),
          velSpread(velocitySpread), rate(rate), distribution(distribution), rng(std::random_device{}()) {}

    void setEnabled(bool enabled) { active = enabled; }
    bool enabled() const { return active; }
    void emit(double dt, std::vector<std::unique_ptr<Particle>>& output, size_t limit) {
        if (!active || dt <= 0) return;
        accumulated += rate * static_cast<float>(dt);
        const int births = static_cast<int>(accumulated);
        accumulated -= births;
        for (int i = 0; i < births && output.size() < limit; ++i) {
            const Vector3D p(sample(origin.x,posSpread.x), sample(origin.y,posSpread.y), sample(origin.z,posSpread.z));
            const Vector3D v(sample(meanVelocity.x,velSpread.x), sample(meanVelocity.y,velSpread.y), sample(meanVelocity.z,velSpread.z));
            output.emplace_back(new Particle(p, v, prototype->getMass(), prototype->getDamping(),
                prototype->getLifetime(), prototype->getRadius(), prototype->getColor()));
        }
    }
private:
    float sample(float mean, float spread) {
        if (spread <= 0.f) return mean;
        if (distribution == Distribution::Gaussian) {
            std::normal_distribution<float> d(mean, spread);
            return d(rng);
        }
        std::uniform_real_distribution<float> d(mean-spread, mean+spread);
        return d(rng);
    }
    std::unique_ptr<Particle> prototype;
    Vector3D origin, meanVelocity, posSpread, velSpread;
    float rate, accumulated = 0.f;
    Distribution distribution;
    std::mt19937 rng;
    bool active = true;
};
