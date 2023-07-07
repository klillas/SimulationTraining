#pragma once

#include "IPhysicsEngine.h"
#include "Physics/RigidObjects/GasMolecules.h"

#include <glm/glm.hpp>
#include <vector>

using namespace Physics::RigidObjects;

namespace Physics::Engine
{
	class PhysicsEngine : public IPhysicsEngine
	{
	public:
		PhysicsEngine();

		void AddGasMolecule(GasMolecules::GasMolecule* rigidObject);

		unsigned GasMoleculeCount();

		void PhysicsTick(float timeDelta);

		std::vector<VulkanInit::Vertex> GetVertices();

	private:
		void ResolveWallCollisions(float timeDelta);
		void ResolveMoleculeCollisions(float timeDelta);

	private:
		std::vector<GasMolecules::GasMolecule*> m_gasMolecules;
		std::vector<VulkanInit::Vertex> m_vertices;
	};
};

