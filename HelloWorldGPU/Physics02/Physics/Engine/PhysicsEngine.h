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
	class PhysicsEngine_ThreadWorker;

	class PhysicsEngine : public IPhysicsEngine
	{
	public:
		PhysicsEngine();

		void AddGasMolecule(GasMolecules::GasMolecule* rigidObject);

		unsigned GasMoleculeCount();

		void PhysicsTick(float timeDelta, unsigned subticks);

		void GetVertices(std::vector<VulkanInit::Vertex>* verticesBuffer);

	public:
		void ResolveMoleculeCollisions(unsigned xIndex, float timeDelta);

	private:
		void ResolveWallCollisions(float timeDelta);

		void ResolveMoleculeCollisions(float timedelta);
		void ResolveMoleculeCollisions(GasMolecules::GasMolecule* molecule, SpaceGridMolecules::Cell* cell);
		
		void UpdateMoleculeCellPositions();

		void CalculateMoleculesPhysicsTick(float timeDelta);

	private:
		std::vector<GasMolecules::GasMolecule*> m_gasMolecules;
		std::vector<VulkanInit::Vertex> m_vertices;
		SpaceGridMolecules m_grid;
		PhysicsEngine_ThreadWorker* m_workers[PhysicsConfiguration::PhysicsEngineWorkerThreads];
		static void PhysicsEngine_ThreadWorker_WorkDoneCallback();
	};
};

