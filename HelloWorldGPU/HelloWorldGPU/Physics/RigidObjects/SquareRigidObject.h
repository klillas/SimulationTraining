#pragma once

#include "IRigidObject.h"
#include "./Physics/Shapes/Square.h"

namespace Physics::RigidObjects
{
	class SquareRigidObject : public Physics::Shapes::Square, public IRigidObject
	{
	public:
		SquareRigidObject(float size, glm::vec2 position, float rotation, float mass, glm::vec2 velocity);
		SquareRigidObject();

		float GetMass();
		void SetMass(float newMass);
		glm::vec2 GetVelocity();
		void SetVelocity(glm::vec2 newVelocity);
		void PhysicsTick(float timeDelta);

		float m_mass;
		glm::vec2 m_velocity;
	};
};

