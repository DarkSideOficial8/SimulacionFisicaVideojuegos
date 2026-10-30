#pragma once

#include "Scene.h"
#include "RenderUtils.hpp"
#include "Particle.h"
#include <vector>
#include <iostream>

class Scene1 : public Scene
{
public:

	explicit Scene1(std::string name)
		: Scene(std::move(name))
	{
	}

	void init() override
	{
		updateProjectileParameters();
	}

	void update(double dt) override
	{
		for (Particle* p : particles)
		{
			p->addForce(Vector3D(0.f, -simGravity, 0.f) * p->getMass());
			p->integrateEuler(dt);
		}
	}

	void keyPress(unsigned char key, const physx::PxTransform& camera) override
	{
		if (key == 'v')
		{
			shoot(camera, key);
		}
		else if (key == 'b')
		{
			shoot(camera, key);
		}

		else if (key == '+')
		{
			realMass += 0.001f;

			updateProjectileParameters();
		}

		else if (key == '-')
		{
			realMass -= 0.001f;

			if (realMass < 0.001f)
				realMass = 0.001f;

			updateProjectileParameters();
		}

		else if (key == 'm')
		{
			realSpeed += 50.0f;

			updateProjectileParameters();
		}

		else if (key == 'n')
		{
			realSpeed -= 50.0f;

			if (realSpeed < 50.0f)
				realSpeed = 50.0f;

			updateProjectileParameters();
		}
	}

	void cleanup() override
	{
		for (Particle* p : particles)
		{
			delete p;
		}

		particles.clear();
	}

private:

	void updateProjectileParameters()
	{
		simMass = realMass * (realSpeed / simSpeed) * (realSpeed / simSpeed);
		simGravity = 9.81f * (simSpeed / realSpeed) * (simSpeed / realSpeed);
	}

	void shoot(const physx::PxTransform& camera, char type)
	{
		Vector3D pos = camera.p;
		Vector3D dir = camera.q.rotate(physx::PxVec3(0.0f, 0.0f, -1.0f));
		dir = dir.normalize();

		Vector3D velocity = dir * simSpeed;


		if (type == 'v')
		{
			Particle* projectile = new Particle(pos, velocity, simMass, 1.0f, physx::PxSphereGeometry(0.2f));
			particles.push_back(projectile);
		}
		else if (type == 'b')
		{
			Particle* projectile = new Particle(pos, velocity, simMass, 1.0f, physx::PxBoxGeometry(0.2f, 0.2f, 0.2f));
			particles.push_back(projectile);
		}

	}


private:

	std::vector<Particle*> particles;

	float realMass = 0.005f;
	float realSpeed = 500.0f;

	float simSpeed = 15.0f;
	float simMass = 0.0f;
	float simGravity = 0.0f;
};