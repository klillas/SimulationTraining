#pragma once

#include <glm/glm.hpp>
#include "IShape.h"

namespace Physics
{
	class IRigidObject : public Shapes::IShape
	{
		float GetMass();
		void SetMass();

		glm::vec2 GetVelocity();
		void SetVelocity(glm::vec2 newVelocity);

		void PhysicsTick(float timeDelta);
	};
}

