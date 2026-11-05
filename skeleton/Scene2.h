#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include <memory>

// Practica 2: fuente, manguera y niebla; cada una es un sistema independiente.
class Scene2 : public Scene {
public:
    explicit Scene2(std::string name) : Scene(std::move(name)) {}
    void init() override {
        cleanup();
        using D = ParticleGenerator::Distribution;
        waterGravity.reset(new GravityForce(Vector3D(0.f, -10.f, 0.f)));
        fogGravity.reset(new GravityForce(Vector3D(0.f, -0.08f, 0.f)));
        fountain.reset(new ParticleSystem());
        hose.reset(new ParticleSystem());
        fog.reset(new ParticleSystem());
        fountain->addGenerator(std::unique_ptr<ParticleGenerator>(new ParticleGenerator(
            Vector3D(-12.f, 0.f, -15.f), Vector3D(0.f, 16.f, 0.f),
            Vector3D(), Vector3D(2.5f, 1.5f, 2.5f), 65.f, D::Gaussian,
            1.f, 1.f, 3.5f, 0.16f, Vector4(0.1f, 0.5f, 1.f, 1.f))));
        hose->addGenerator(std::unique_ptr<ParticleGenerator>(new ParticleGenerator(
            Vector3D(0.f, 3.f, -15.f), Vector3D(11.f, 8.f, 0.f),
            Vector3D(), Vector3D(1.5f, 1.f, 1.5f), 85.f, D::Uniform,
            1.f, 1.f, 3.f, 0.12f, Vector4(0.2f, 1.f, 0.4f, 1.f))));
        fog->addGenerator(std::unique_ptr<ParticleGenerator>(new ParticleGenerator(
            Vector3D(12.f, 6.f, -15.f), Vector3D(0.f, 0.18f, 0.f),
            Vector3D(4.f, 1.5f, 4.f), Vector3D(0.18f, 0.1f, 0.18f),
            45.f, D::Gaussian, 1.f, 0.85f, 3.5f, 0.35f,
            Vector4(0.82f, 0.85f, 0.92f, 0.35f))));
        fountain->addForceGenerator(waterGravity.get());
        hose->addForceGenerator(waterGravity.get());
        fog->addForceGenerator(fogGravity.get());
    }
    void update(double dt) override {
        fountain->update(dt);
        hose->update(dt);
        fog->update(dt);
    }
    void keyPress(unsigned char key, const physx::PxTransform&) override {
        if (key == '5') { fountainEnabled = !fountainEnabled; fountain->setGeneratorsEnabled(fountainEnabled); }
        else if (key == '6') { hoseEnabled = !hoseEnabled; hose->setGeneratorsEnabled(hoseEnabled); }
        else if (key == '7') { fogEnabled = !fogEnabled; fog->setGeneratorsEnabled(fogEnabled); }
        else if (key == 'r') init();
    }
    void cleanup() override {
        fountain.reset(); hose.reset(); fog.reset();
        waterGravity.reset(); fogGravity.reset();
        fountainEnabled = hoseEnabled = fogEnabled = true;
    }
private:
    std::unique_ptr<ParticleSystem> fountain, hose, fog;
    std::unique_ptr<GravityForce> waterGravity, fogGravity;
    bool fountainEnabled = true, hoseEnabled = true, fogEnabled = true;
};
