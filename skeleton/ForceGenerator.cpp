#include "ForceGenerator.h"
#include <cmath>

bool WindForce::inside(const Vector3D& p) const {
    return std::fabs(p.x - center.x) <= halfExtents.x &&
        std::fabs(p.y - center.y) <= halfExtents.y &&
        std::fabs(p.z - center.z) <= halfExtents.z;
}
void WindForce::updateForce(Particle& p, double) {
    Vector3D pos = p.position();
    if (!inside(pos)) return;
    Vector3D relative = windAt(pos) - p.velocity();
    float speed = relative.magnitude();
    p.addForce(relative * (k1 + k2 * speed));
}
Vector3D VortexForce::windAt(const Vector3D& pos) const {
    // Tornado: giro alrededor del eje Y, succion hacia el centro y ascenso.
    Vector3D d = pos - center;
    const float radial = std::sqrt(d.x * d.x + d.z * d.z);
    if (radial < 0.001f) return Vector3D(0.f, 22.f, 0.f);
    const Vector3D tangent(-d.z / radial, 0.f, d.x / radial);
    const Vector3D inward(-d.x / radial, 0.f, -d.z / radial);
    // Giro mas intenso cerca del centro, con elevacion permanente.
    const float spin = strength * (0.55f + 0.45f * (1.f - radial / radius));
    return tangent * spin + inward * 7.f + Vector3D(0.f, 22.f, 0.f);
}
void VortexForce::updateForce(Particle& p, double dt) {
    Vector3D d = Vector3D(p.position()) - center;
    const float radial = std::sqrt(d.x * d.x + d.z * d.z);
    // Cilindro vertical: permite que el tornado eleve las particulas.
    if (radial > radius || d.y < -25.f || d.y > 35.f) return;
    Vector3D relative = windAt(Vector3D(p.position())) - p.velocity();
    const float speed = relative.magnitude();
    p.addForce(relative * (k1 + k2 * speed));
}
void ExplosionForce::updateForce(Particle& p, double) {
    if (!active || tau <= 0.f) return;
    Vector3D d = Vector3D(p.position()) - center;
    float r = d.magnitude();
    if (r >= radius || r < 0.01f) return;
    // F = (K / r^2) * (pos-centro) * exp(-t/tau)
    float factor = strength / (r * r + 9.f) * std::exp(-elapsed / tau);
    p.addForce(d * factor);
}
