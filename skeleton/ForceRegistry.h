#pragma once
#include "ForceGenerator.h"
#include <vector>

// No posee ni destruye las particulas ni los generadores.
class ForceRegistry {
public:
    struct Registration { Particle* particle; ForceGenerator* generator; };
    void add(Particle* p, ForceGenerator* g) {
        if (p && g) registrations.push_back({ p, g });
    }
    void clear() { registrations.clear(); }
    void updateForces(double dt) {
        for (const auto& entry : registrations)
            entry.generator->updateForce(*entry.particle, dt);
    }
private:
    std::vector<Registration> registrations;
};

