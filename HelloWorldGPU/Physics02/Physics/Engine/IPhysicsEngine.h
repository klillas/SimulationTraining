#pragma once

#include "Physics/RigidObjects/GasMolecules.h"

using namespace Physics::RigidObjects;

namespace Physics::Engine
{
	class IPhysicsEngine
	{
	public:
		virtual void AddGasMolecule(GasMolecules::GasMolecule* rigidObject) = 0;

		virtual void PhysicsTick(float timeDelta) = 0;

		virtual std::vector<VulkanInit::Vertex> GetVertices() = 0;
	};
};

