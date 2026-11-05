#pragma once
#include "ParticleGenerator.h"
#include "ForceRegistry.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

// Posee particulas y emisores; los generadores de fuerza son externos y compartibles.
class ParticleSystem {
public:
    explicit ParticleSystem(size_t limit = 2500) : maxParticles(limit) {}
    ParticleGenerator* addGenerator(std::unique_ptr<ParticleGenerator> emitter) {
        ParticleGenerator* result = emitter.get();
        generators.emplace_back(std::move(emitter));
        return result;
    }
    void setGeneratorsEnabled(bool enabled) {
        for (auto& generator : generators) generator->setEnabled(enabled);
    }
    void addParticle(std::unique_ptr<Particle> particle) {
        if (particle && particles.size() < maxParticles) particles.emplace_back(std::move(particle));
    }
    void addForceGenerator(ForceGenerator* force) {
        if (force && std::find(forces.begin(), forces.end(), force) == forces.end()) forces.push_back(force);
    }
    void removeForceGenerator(ForceGenerator* force) {
        forces.erase(std::remove(forces.begin(), forces.end(), force), forces.end());
    }
    void clear() { registry.clear(); particles.clear(); generators.clear(); forces.clear(); }
    size_t count() const { return particles.size(); }
    void update(double dt) {
        if (dt <= 0) return;
        for (auto& generator : generators) generator->emit(dt, particles, maxParticles);
        registry.clear();
        for (auto& particle : particles) {
            particle->clearForce();
            for (auto* force : forces) registry.add(particle.get(), force);
        }
        registry.updateForces(dt); // cada Particle llama a su ForceGenerator
        for (auto& particle : particles) particle->integrateEuler(dt);
        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [](const std::unique_ptr<Particle>& particle) {
                if (!particle || particle->isDead()) return true;
                const physx::PxVec3 pos = particle->position();
                return pos.y < -25.f || pos.y > 100.f ||
                    std::fabs(pos.x) > 150.f || std::fabs(pos.z) > 150.f;
            }), particles.end());
    }
private:
    size_t maxParticles;
    ForceRegistry registry;
    std::vector<std::unique_ptr<Particle>> particles;
    std::vector<std::unique_ptr<ParticleGenerator>> generators;
    std::vector<ForceGenerator*> forces;
};
