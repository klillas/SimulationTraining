#pragma once

#include "./Physics/RigidObjects/IRigidObject.h"
#include "IPhysicsEngine.h"
#include "./Physics/RigidObjects/IRigidObject.h"

#include <glm/glm.hpp>
#include <vector>

using namespace Physics::RigidObjects;

namespace Physics::Engine
{
	class PhysicsEngine : public IPhysicsEngine
	{
	public:
		PhysicsEngine();

		void AddRigidObject(IRigidObject* rigidObject);

		void PhysicsTick(float timeDelta);

		std::vector<VulkanInit::Vertex> GetVertices();

	private:
		void TickCollisionDetection(float timeDelta);

	private:
		std::vector<IRigidObject*> rigidObjects;
		std::vector<VulkanInit::Vertex> m_vertices;
	};
};

