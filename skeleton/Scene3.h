#pragma once
#include "Scene.h"
#include "ForceGenerator.h"
#include "ForceRegistry.h"
#include <memory>
#include <vector>
#include <cmath>

class Scene3 : public Scene {
public:
    explicit Scene3(std::string name) : Scene(std::move(name)) {}
    void init() override {
        cleanup();
        gravityEarth.reset(new GravityForce(Vector3D(0.f, -10.f, 0.f)));
        gravityLight.reset(new GravityForce(Vector3D(0.f, -3.f, 0.f)));
        wind.reset(new WindForce(Vector3D(32.f, 3.f, 0.f),
            Vector3D(0.f, 10.f, -15.f), Vector3D(45.f, 35.f, 35.f), 7.f));
        vortex.reset(new VortexForce(Vector3D(0.f, 10.f, -15.f), 24.f, 28.f, 7.f));
        explosion.reset(new ExplosionForce(Vector3D(0.f, 10.f, -15.f), 32.f, 2500.f, 0.9f));
        // 48 particulas repartidas alrededor del centro de los efectos.
        // La espiral del tornado y la onda de la explosion resultan visibles.
        for (int i = 0; i < 48; ++i) {
            const float angle = 6.2831853f * static_cast<float>(i % 16) / 16.f;
            const float radius = 3.5f + 2.5f * static_cast<float>(i / 16);
            const float y = 7.f + 4.f * static_cast<float>(i / 16);
            const float m = (i % 3 == 0) ? 0.5f : (i % 3 == 1 ? 2.f : 5.f);
            particles.emplace_back(new Particle(
                Vector3D(std::cos(angle) * radius, y, -15.f + std::sin(angle) * radius),
                Vector3D(), m, 1.f, 1000.f, 0.55f,
                Vector4(0.25f + 0.012f * i, 0.65f, 1.f, 1.f)));
        }
    }
    void update(double dt) override {
        // Orden del Tema 05: limpiar -> acumular -> integrar.
        registry.clear();
        for (size_t i = 0; i < particles.size(); ++i) {
            Particle* p = particles[i].get();
            p->clearForce();
            registry.add(p, i < particles.size() / 2 ? static_cast<ForceGenerator*>(gravityEarth.get())
                : static_cast<ForceGenerator*>(gravityLight.get()));
            if (enableWind) registry.add(p, wind.get());
            if (enableVortex) registry.add(p, vortex.get());
            registry.add(p, explosion.get());
        }
        registry.updateForces(dt);
        for (auto& p : particles) p->integrateEuler(dt);
        explosion->advance(dt);
    }
    void keyPress(unsigned char key, const physx::PxTransform&) override {
        if (key == 'e' || key == 'E') {
            // Recolocar particulas antes de la explosion para que no hayan caido ya.
            const bool wasWind = enableWind, wasVortex = enableVortex;
            init();
            enableWind = wasWind; enableVortex = wasVortex;
            explosion->restart();
        }
        if (key == 'v' || key == 'V') enableWind = !enableWind;
        if (key == 't' || key == 'T') enableVortex = !enableVortex;
        if (key == 'r' || key == 'R') init();
    }
    void cleanup() override {
        registry.clear();
        particles.clear();
        explosion.reset(); vortex.reset(); wind.reset();
        gravityLight.reset(); gravityEarth.reset();
        enableWind = false; enableVortex = false;
    }
private:
    ForceRegistry registry;
    std::vector<std::unique_ptr<Particle>> particles;
    std::unique_ptr<GravityForce> gravityEarth, gravityLight;
    std::unique_ptr<WindForce> wind;
    std::unique_ptr<VortexForce> vortex;
    std::unique_ptr<ExplosionForce> explosion;
    bool enableWind = false, enableVortex = false;
};
