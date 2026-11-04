#pragma once
#include "Scene.h"
#include "ParticleSystem.h"
#include "RenderUtils.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cmath>

// Proyecto intermedio: sandbox de desastres. No usa aun cuerpos rigidos ni muelles.
class DisasterScene : public Scene {
public:
    explicit DisasterScene(std::string name) : Scene(std::move(name)) {}
    void init() override {
        cleanup();
        gravity.reset(new GravityForce(Vector3D(0.f, -9.81f, 0.f)));
        projectileGravity.reset(new ProjectileGravityForce());
        wind.reset(new WindForce(Vector3D(28.f, 4.f, 0.f), Vector3D(0.f, 10.f, -15.f),
            Vector3D(55.f, 40.f, 55.f), 6.f));
        tornado.reset(new VortexForce(Vector3D(0.f, 10.f, -15.f), 23.f, 35.f, 8.f));
        explosion.reset(new ExplosionForce(Vector3D(0.f, 8.f, -15.f), 30.f, 3500.f, 1.1f));

        debris.reset(new ParticleSystem(500));
        atmosphere.reset(new ParticleSystem(1500));
        smokeSystem.reset(new ParticleSystem(900)); // Humo sin gravedad: asciende y permanece suspendido.
        shots.reset(new ParticleSystem(100));
        for (ParticleSystem* s : { debris.get(),atmosphere.get(),shots.get() }) {
            s->addForceGenerator(s == shots.get() ? static_cast<ForceGenerator*>(projectileGravity.get()) : static_cast<ForceGenerator*>(gravity.get()));
            s->addForceGenerator(explosion.get());
        }
        // Dos masas distintas, distribuidas en anillos alrededor del tornado.
        for (int i = 0; i < 64; ++i) {
            const float angle = 6.2831853f * (i % 16) / 16.f;
            const float r = 6.f + 3.5f * (i / 16);
            const float mass = (i % 2 == 0) ? 0.7f : 4.f;
            debris->addParticle(std::unique_ptr<Particle>(new Particle(
                Vector3D(std::cos(angle) * r, 8.f + 3.f * (i / 16), -15.f + std::sin(angle) * r),
                Vector3D(), mass, 0.99f, 1000.f, (i % 2 == 0) ? 0.42f : 0.75f,
                (i % 2 == 0) ? Vector4(0.95f, 0.7f, 0.25f, 1.f) : Vector4(0.5f, 0.65f, 0.85f, 1.f))));
        }
        using D = ParticleGenerator::Distribution;
        dust = atmosphere->addGenerator(std::unique_ptr<ParticleGenerator>(new ParticleGenerator(
            Vector3D(0.f, 3.f, -15.f), Vector3D(0.f, 4.f, 0.f),
            Vector3D(10.f, 1.f, 10.f), Vector3D(3.f, 2.f, 3.f),
            40.f, D::Uniform, 0.2f, 0.98f, 5.f, 0.22f, Vector4(0.65f, 0.56f, 0.42f, 1.f))));
        // Humo gaussiano: nace en el tejado, asciende lentamente y se dispersa.
        // Sistema separado SIN gravedad: evita el efecto de chorro que sube y cae.
        // Tejado: x=-7, z=6, superficie superior y=6.8.
        smoke = smokeSystem->addGenerator(std::unique_ptr<ParticleGenerator>(new ParticleGenerator(
            Vector3D(-7.f, 7.15f, 6.f), Vector3D(0.f, 2.2f, 0.f),
            Vector3D(0.15f, 0.08f, 0.15f), Vector3D(0.9f, 0.45f, 0.9f),
            55.f, D::Gaussian, 0.12f, 0.90f, 6.f, 0.23f, Vector4(0.58f, 0.59f, 0.62f, 1.f))));
        dust->setEnabled(false); smoke->setEnabled(false);
        shots->onImpact([this](const Projectile& p, const Vector3D& pos) {
            if (explosionEnabled) {
                explosion->setCenter(pos);
                explosion->restart();
            }
            std::cout << "[Impacto] " << (p.type() == Projectile::Type::Meteorite ? "Meteorito" : "Bola de choque")
                << " energia real=" << p.realEnergy() << " J; energia sim=" << p.simulationEnergy() << " J\n";
            });
        buildEnvironment();
        printControls();
    }
    void update(double dt) override {
        if (dt <= 0 || dt > 0.2) return;
        debris->update(dt);
        atmosphere->update(dt);
        smokeSystem->update(dt);
        shots->update(dt);
        explosion->advance(dt);
    }
    void keyPress(unsigned char key, const physx::PxTransform& camera) override {
        switch (key) {
        case 'm': case 'M': shoot(Projectile::Type::Meteorite, camera); break;
        case 'b': case 'B': shoot(Projectile::Type::ShockBall, camera); break;
        case 'h': case 'H': windEnabled = !windEnabled; setForce(wind.get(), windEnabled); break;
        case 't': case 'T': tornadoEnabled = !tornadoEnabled; setForce(tornado.get(), tornadoEnabled); break;
        case 'g': case 'G': gravityEnabled = !gravityEnabled; setForce(gravity.get(), gravityEnabled); break;
        case 'e': case 'E': explosionEnabled = !explosionEnabled;
            if (explosionEnabled) { explosion->setCenter(Vector3D(0.f, 10.f, -15.f)); explosion->restart(); }
            else explosion->stop(); break;
        case '5': dust->setEnabled(!dust->enabled()); break;
        case '6': smoke->setEnabled(!smoke->enabled()); break;
        case 'r': case 'R': init(); return;
        default: return;
        }
        std::cout << "[Desastres] viento=" << windEnabled << " tornado=" << tornadoEnabled
            << " gravedad=" << gravityEnabled << " explosion=" << explosionEnabled
            << " polvo=" << dust->enabled() << " humo=" << smoke->enabled() << "\n";
    }
    void cleanup() override {
        // Primero destruir particulas y registros; despues los generadores externos.
        shots.reset(); smokeSystem.reset(); atmosphere.reset(); debris.reset();
        for (auto& obj : environment) if (obj->item) obj->item->release();
        environment.clear();
        explosion.reset(); tornado.reset(); wind.reset(); gravity.reset(); projectileGravity.reset();
        dust = nullptr; smoke = nullptr;
        windEnabled = false; tornadoEnabled = false; gravityEnabled = true; explosionEnabled = true;
    }
private:
    struct StaticObject { physx::PxTransform pose; RenderItem* item = nullptr; };
    void box(const Vector3D& pos, const Vector3D& half, const Vector4& color) {
        std::unique_ptr<StaticObject> obj(new StaticObject());
        obj->pose = physx::PxTransform(static_cast<physx::PxVec3>(pos));
        physx::PxShape* shape = CreateShape(physx::PxBoxGeometry(half.x, half.y, half.z));
        obj->item = new RenderItem(shape, &obj->pose, color);
        shape->release();
        environment.emplace_back(std::move(obj));
    }
    void buildEnvironment() {
        box(Vector3D(0.f, -0.8f, -15.f), Vector3D(45.f, 0.7f, 40.f), Vector4(0.23f, 0.43f, 0.22f, 1.f));
        // Maquetas estaticas: no se destruyen hasta el tema de solidos rigidos.
        for (int i = 0;i < 8;++i) {
            const float x = (i % 4 - 1.5f) * 14.f;
            const float z = -38.f + (i / 4) * 44.f;
            box(Vector3D(x, 3.f, z), Vector3D(3.f, 3.f, 3.f), Vector4(0.5f + 0.04f * (i % 4), 0.52f, 0.55f, 1.f));
            box(Vector3D(x, 6.4f, z), Vector3D(3.4f, 0.4f, 3.4f), Vector4(0.34f, 0.27f, 0.25f, 1.f));
        }
    }
    void setForce(ForceGenerator* force, bool enabled) {
        for (ParticleSystem* s : { debris.get(),atmosphere.get(),shots.get(),smokeSystem.get() }) {
            if (force == gravity.get() && s == smokeSystem.get()) continue;
            ForceGenerator* target = (force == gravity.get() && s == shots.get())
                ? static_cast<ForceGenerator*>(projectileGravity.get()) : force;
            if (enabled) s->addForceGenerator(target);
            else s->removeForceGenerator(target);
        }
    }
    void shoot(Projectile::Type type, const physx::PxTransform& camera) {
        Vector3D dir(camera.q.rotate(physx::PxVec3(0.f, 0.f, -1.f)));
        const float len = dir.magnitude();
        if (len < 0.001f) return;
        dir = dir * (1.f / len);
        // Escalado de velocidad, masa, energia y fuerza de la practica 1.
        const float mass = type == Projectile::Type::Meteorite ? 0.012f : 0.003f;
        const float realSpeed = type == Projectile::Type::Meteorite ? 450.f : 700.f;
        const float simSpeed = type == Projectile::Type::Meteorite ? 26.f : 38.f;
        shots->addProjectile(std::unique_ptr<Projectile>(new Projectile(type, Vector3D(camera.p), dir,
            mass, realSpeed, simSpeed)));
    }
    void printControls() const {
        std::cout << "\n=== DISASTER SIMULATOR - PROYECTO INTERMEDIO ===\n"
            << "M: meteorito desde camara | B: bola de choque desde camara\n"
            << "H: huracan/viento ON/OFF | T: tornado ON/OFF\n"
            << "G: gravedad ON/OFF | E: explosion ON/OFF (y detonar)\n"
            << "5: polvo uniforme ON/OFF | 6: humo gaussiano ON/OFF\n"
            << "R: reiniciar | 0/1/2: escenas de practicas | 4: este proyecto\n"
            << "Los edificios son decorativos; la destruccion rigida llegara mas adelante.\n";
    }
    std::unique_ptr<GravityForce> gravity;
    std::unique_ptr<ProjectileGravityForce> projectileGravity;
    std::unique_ptr<WindForce> wind;
    std::unique_ptr<VortexForce> tornado;
    std::unique_ptr<ExplosionForce> explosion;
    std::unique_ptr<ParticleSystem> debris, atmosphere, shots, smokeSystem;
    ParticleGenerator* dust = nullptr, * smoke = nullptr;
    std::vector<std::unique_ptr<StaticObject>> environment;
    bool windEnabled = false, tornadoEnabled = false, gravityEnabled = true, explosionEnabled = true;
};
