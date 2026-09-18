#pragma once

#include "Scene.h"
#include "RenderUtils.hpp"
#include <vector>

class Scene1 : public Scene {
public:
	explicit Scene1(std::string name) : Scene(std::move(name)) {}

	void init() override
	{
		physx::PxShape* shape = CreateShape(physx::PxSphereGeometry(2.0f));
		m_transform = physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f));

		m_renderItem = new RenderItem(shape, &m_transform, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}
	void cleanup() override {
		if (m_renderItem) {
			m_renderItem->release();
			m_renderItem = nullptr;
		}
	}
	virtual void update(double dt) override{}

private:
	physx::PxTransform m_transform;
	RenderItem* m_renderItem{ nullptr };

};