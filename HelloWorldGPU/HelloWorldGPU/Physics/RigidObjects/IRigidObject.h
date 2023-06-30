#pragma once

#include <glm/glm.hpp>
#include "./Physics/Shapes/IShape.h"

namespace Physics::RigidObjects
{
	class IRigidObject
	{
	public:
		virtual float GetMass() = 0;
		virtual void SetMass(float newMass) = 0;

		virtual glm::vec2 GetVelocity() = 0;
		virtual void SetVelocity(glm::vec2 newVelocity) = 0;
		virtual void AddVelocity(glm::vec2 addedVelocity) = 0;

		virtual void PhysicsTick(float timeDelta) = 0;
	};
}

