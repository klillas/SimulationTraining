#pragma once

#include "IPhysicsEngine.h"
#include "Physics/RigidObjects/GasMolecules.h"
#include "Physics/SpaceGrid/SpaceGridMolecules.h"

#include <glm/glm.hpp>
#include <vector>

using namespace Physics::RigidObjects;
using namespace Physics::SpaceGrid;

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
		void ResolveMoleculeCollisions(GasMolecules::GasMolecule* molecule, SpaceGridMolecules::Cell* cell);
		void UpdateMoleculeCellPositions();

	private:
		std::vector<GasMolecules::GasMolecule*> m_gasMolecules;
		std::vector<VulkanInit::Vertex> m_vertices;
		SpaceGridMolecules m_grid;
	};
};

