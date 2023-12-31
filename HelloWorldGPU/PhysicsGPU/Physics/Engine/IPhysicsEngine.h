#pragma once

#include "Physics/RigidObjects/GasMolecules.h"

using namespace Physics::RigidObjects;

namespace Physics::Engine
{
	class IPhysicsEngine
	{
	public:
		virtual void AddGasMolecule(GasMolecules::GasMolecule* rigidObject) = 0;

		virtual unsigned GasMoleculeCount() = 0;

		virtual void PhysicsTick(float timeDelta, unsigned subticks) = 0;

		virtual void GetVertices(std::vector<VulkanInit::Vertex>* verticesBuffer) = 0;
	};
};

