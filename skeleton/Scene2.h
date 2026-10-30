#pragma once
#include "Scene.h"
#include "ParticleEmitter.h"
#include <memory>
#include <vector>
#include <utility>

class Scene2 : public Scene {
public:
    explicit Scene2(std::string name) : Scene(std::move(name)) {}
    void init() override {
        cleanup();
        using D = ParticleEmitter::Distribution;
        // Fuente de agua: chorro ascendente con dispersión gaussiana.
        emitters.emplace_back(new ParticleEmitter(
            Vector3D(-12.f, 0.f, -15.f), Vector3D(0.f, 16.f, 0.f),
            Vector3D(2.5f, 1.5f, 2.5f), 65.f, 3.5f, 0.16f,
            Vector4(0.1f, 0.5f, 1.f, 1.f), D::Gaussian));
        // Manguera: chorro lateral, velocidad uniforme.
        emitters.emplace_back(new ParticleEmitter(
            Vector3D(0.f, 3.f, -15.f), Vector3D(11.f, 8.f, 0.f),
            Vector3D(1.5f, 1.0f, 1.5f), 85.f, 3.0f, 0.12f,
            Vector4(0.2f, 1.f, 0.4f, 1.f), D::Uniform));
        // Niebla: posiciones dispersas y velocidades muy pequeñas.
        emitters.emplace_back(new ParticleEmitter(
            Vector3D(12.f, 6.f, -15.f), Vector3D(0.f, 0.18f, 0.f),
            Vector3D(0.18f, 0.10f, 0.18f), 45.f, 3.5f, 0.35f,
            Vector4(0.82f, 0.85f, 0.92f, 0.35f), D::Gaussian,
            Vector3D(4.f, 1.5f, 4.f), 0.85f, Vector3D(0.f, -0.08f, 0.f)));
    }
    void update(double dt) override {
        for (auto& emitter : emitters) emitter->update(dt);
    }
    void cleanup() override { emitters.clear(); }
private:
    std::vector<std::unique_ptr<ParticleEmitter>> emitters;
};

