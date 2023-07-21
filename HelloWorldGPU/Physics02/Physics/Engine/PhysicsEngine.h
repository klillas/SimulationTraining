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

	private:
		std::vector<GasMolecules::GasMolecule*> m_gasMolecules;
		std::vector<VulkanInit::Vertex> m_vertices;
		SpaceGridMolecules m_grid;
		PhysicsEngine_ThreadWorker *m_worker1;
		PhysicsEngine_ThreadWorker* m_worker2;
		PhysicsEngine_ThreadWorker* m_worker3;
		PhysicsEngine_ThreadWorker* m_worker4;
		PhysicsEngine_ThreadWorker* m_worker5;
		PhysicsEngine_ThreadWorker* m_worker6;
		PhysicsEngine_ThreadWorker* m_worker7;
		PhysicsEngine_ThreadWorker* m_worker8;
		static void PhysicsEngine_ThreadWorker_WorkDoneCallback();
	};
};

