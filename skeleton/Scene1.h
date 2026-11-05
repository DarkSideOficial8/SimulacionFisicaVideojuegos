#pragma once
#include "Scene.h"
#include "Projectile.h"
#include <memory>
#include <vector>

// Practica 1: dos tipos de proyectiles desde la camara y escalado energetico.
class Scene1 : public Scene {
public:
    explicit Scene1(std::string name) : Scene(std::move(name)) {}
    void init() override { cleanup(); }
    void update(double dt) override {
        for (auto& p : projectiles) {
            p->clearForce();
            p->addForce(Vector3D(0.f, -p->gravity() * p->getMass(), 0.f));
            p->integrateEuler(dt);
            p->syncRender();
        }
    }
    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        if (key == 'v' || key == 'b') {
            Vector3D direction = camera.q.rotate(physx::PxVec3(0.f, 0.f, -1.f));
            direction = direction.normalize();
            projectiles.emplace_back(new Projectile(
                key == 'v' ? Projectile::Type::Sphere : Projectile::Type::Box,
                Vector3D(camera.p), direction, realMass, realSpeed, simSpeed));
        } else if (key == '+') realMass += 0.001f;
        else if (key == '-') { realMass -= 0.001f; if (realMass < 0.001f) realMass = 0.001f; }
        else if (key == 'm') realSpeed += 50.f;
        else if (key == 'n') { realSpeed -= 50.f; if (realSpeed < 50.f) realSpeed = 50.f; }
        else if (key == 'r') init();
    }
    void cleanup() override { projectiles.clear(); }
private:
    std::vector<std::unique_ptr<Projectile>> projectiles;
    float realMass = 100.f, realSpeed = 500.f, simSpeed = 7.f;
};
