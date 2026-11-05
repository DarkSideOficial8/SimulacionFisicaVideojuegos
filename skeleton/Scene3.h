#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include <memory>
#include <cmath>

// Practica 3: dos sistemas con gravedades diferentes y fuerzas compartidas.
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
        earth.reset(new ParticleSystem());
        light.reset(new ParticleSystem());
        earth->addForceGenerator(gravityEarth.get());
        light->addForceGenerator(gravityLight.get());
        // Los mismos generadores externos pueden actuar en ambos sistemas.
        for (auto* system : {earth.get(), light.get()})
            system->addForceGenerator(explosion.get());
        for (int i = 0; i < 48; ++i) {
            const float angle = 6.2831853f * static_cast<float>(i % 16) / 16.f;
            const float radius = 3.5f + 2.5f * static_cast<float>(i / 16);
            const float y = 7.f + 4.f * static_cast<float>(i / 16);
            const float m = (i % 3 == 0) ? 0.5f : (i % 3 == 1 ? 2.f : 5.f);
            std::unique_ptr<Particle> p(new Particle(
                Vector3D(std::cos(angle) * radius, y, -15.f + std::sin(angle) * radius),
                Vector3D(), m, 1.f, 1000.f, 0.55f,
                Vector4(0.25f + 0.012f * i, 0.65f, 1.f, 1.f)));
            (i < 24 ? earth : light)->addParticle(std::move(p));
        }
    }
    void update(double dt) override {
        earth->update(dt);
        light->update(dt);
        explosion->advance(dt);
    }
    void keyPress(unsigned char key, const physx::PxTransform&) override {
        if (key == 'e') {
            if (explosion->isActive()) explosion->stop();
            else {
                const bool oldWind = enableWind, oldVortex = enableVortex, oldGravity = enableGravity;
                init();
                if (oldWind) toggleWind();
                if (oldVortex) toggleVortex();
                if (!oldGravity) toggleGravity();
                explosion->restart();
            }
        } else if (key == 'g') toggleGravity();
        else if (key == 'v') toggleWind();
        else if (key == 't') toggleVortex();
        else if (key == 'r') init();
    }
    void cleanup() override {
        earth.reset(); light.reset();
        explosion.reset(); vortex.reset(); wind.reset();
        gravityLight.reset(); gravityEarth.reset();
        enableWind = false; enableVortex = false; enableGravity = true;
    }
private:
    void toggleGravity() {
        enableGravity = !enableGravity;
        if (enableGravity) {
            earth->addForceGenerator(gravityEarth.get());
            light->addForceGenerator(gravityLight.get());
        } else {
            earth->removeForceGenerator(gravityEarth.get());
            light->removeForceGenerator(gravityLight.get());
        }
    }
    void toggleWind() {
        enableWind = !enableWind;
        for (auto* system : {earth.get(), light.get()})
            if (enableWind) system->addForceGenerator(wind.get());
            else system->removeForceGenerator(wind.get());
    }
    void toggleVortex() {
        enableVortex = !enableVortex;
        for (auto* system : {earth.get(), light.get()})
            if (enableVortex) system->addForceGenerator(vortex.get());
            else system->removeForceGenerator(vortex.get());
    }
    std::unique_ptr<ParticleSystem> earth, light;
    std::unique_ptr<GravityForce> gravityEarth, gravityLight;
    std::unique_ptr<WindForce> wind;
    std::unique_ptr<VortexForce> vortex;
    std::unique_ptr<ExplosionForce> explosion;
    bool enableWind = false, enableVortex = false, enableGravity = true;
};
