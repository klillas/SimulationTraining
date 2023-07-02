#pragma once

#include "IRigidObject.h"
#include "./Physics/Shapes/GasMolecule.h"
#include "./Physics/Shapes/IShape.h"

namespace Physics::RigidObjects
{
	class GasMoleculeRigidObject : public Physics::Shapes::GasMolecule, public IRigidObject
	{
	public:
		GasMoleculeRigidObject(glm::vec2 position, glm::vec2 velocity);
		GasMoleculeRigidObject();

		glm::vec2 GetVelocity();
		void SetVelocity(glm::vec2 newVelocity);
		void PhysicsTick(float timeDelta);
		void AddVelocity(glm::vec2 addedVelocity);

		glm::vec2 m_velocity;
	};
};

