#pragma once
#include "ParticleGenerator.h"
#include "ForceRegistry.h"
#include "Projectile.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_set>

// Un sistema posee sus particulas y emisores, y registra las fuerzas que le afectan.
// Los ForceGenerator son externos y se pueden compartir entre sistemas.
class ParticleSystem {
public:
    explicit ParticleSystem(size_t limit = 1500) : maxParticles(limit) {}
    ParticleGenerator* addGenerator(std::unique_ptr<ParticleGenerator> emitter) {
        ParticleGenerator* result = emitter.get();
        generators.emplace_back(std::move(emitter));
        return result;
    }
    void addParticle(std::unique_ptr<Particle> p) { if (particles.size() < maxParticles) particles.emplace_back(std::move(p)); }
    // Los proyectiles se registran explicitamente, sin RTTI/dynamic_cast.
    void addProjectile(std::unique_ptr<Projectile> projectile) {
        if (!projectile || particles.size() >= maxParticles) return;
        projectilePointers.insert(projectile.get());
        particles.emplace_back(std::move(projectile));
    }
    void addForceGenerator(ForceGenerator* force) {
        if (force && std::find(forces.begin(), forces.end(), force) == forces.end()) forces.push_back(force);
    }
    void removeForceGenerator(ForceGenerator* force) {
        forces.erase(std::remove(forces.begin(), forces.end(), force), forces.end());
    }
    void clear() { registry.clear(); particles.clear(); projectilePointers.clear(); generators.clear(); forces.clear(); }
    size_t count() const { return particles.size(); }
    void onImpact(std::function<void(const Projectile&, const Vector3D&)> cb) { impactCallback = std::move(cb); }
    void update(double dt) {
        if (dt <= 0.0) return;
        for (auto& gen : generators) gen->emit(dt, particles, maxParticles);
        registry.clear();
        for (auto& p : particles) {
            p->clearForce();
            for (ForceGenerator* force : forces) registry.add(p.get(), force);
        }
        registry.updateForces(dt);
        for (auto& p : particles) p->integrateEuler(dt);
        // Detectar impactos antes de borrar. No se modifican las colecciones
        // mientras se recorren; el callback solo notifica el impacto.
        if (impactCallback) {
            for (const auto& particle : particles) {
                if (!particle) continue;
                const auto* projectile = projectilePointers.count(particle.get())
                    ? static_cast<const Projectile*>(particle.get()) : nullptr;
                if (projectile && projectile->position().y <= 0.7f)
                    impactCallback(*projectile, Vector3D(projectile->position()));
            }
        }

        // Eliminacion en dos fases: comprobar validez y condiciones, despues borrar.
        // El chequeo !particle evita desreferenciar unique_ptr vacios.
        auto shouldRemove = [this](const std::unique_ptr<Particle>& particle) -> bool {
            if (!particle) return true;
            if (particle->isDead()) return true;
            const physx::PxVec3 pos = particle->position(); // copia, no referencia
            if (pos.y < -6.f || pos.y > 100.f ||
                std::abs(pos.x) > 120.f || std::abs(pos.z) > 120.f)
                return true;
            const bool isProjectile = projectilePointers.count(particle.get()) != 0;
            return isProjectile && pos.y <= 0.7f;
            };
        particles.erase(std::remove_if(particles.begin(), particles.end(),
            [this, &shouldRemove](const std::unique_ptr<Particle>& particle) {
                if (!shouldRemove(particle)) return false;
                projectilePointers.erase(particle.get());
                return true;
            }), particles.end());
    }
private:
    size_t maxParticles;
    std::function<void(const Projectile&, const Vector3D&)> impactCallback;
    ForceRegistry registry;
    std::vector<std::unique_ptr<Particle>> particles;
    std::vector<std::unique_ptr<ParticleGenerator>> generators;
    std::vector<ForceGenerator*> forces;
    std::unordered_set<const Particle*> projectilePointers;
};
