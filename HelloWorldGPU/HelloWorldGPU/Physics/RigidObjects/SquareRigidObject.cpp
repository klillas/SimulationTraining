#include "SquareRigidObject.h"

using namespace Physics::RigidObjects;

SquareRigidObject::SquareRigidObject(float size, glm::vec2 position, float rotation, float mass, glm::vec2 velocity)
	: Square(size, position, rotation)
	, m_mass(mass)
	, m_velocity(velocity)
{

}

SquareRigidObject::SquareRigidObject()
	: Square(),
	m_mass(1.0f),
	m_velocity(0.0f, 0.0f)
{

}

float SquareRigidObject::GetMass()
{
	return m_mass;
}

void SquareRigidObject::SetMass(float newMass)
{
	m_mass = newMass;
}

glm::vec2 SquareRigidObject::GetVelocity()
{
	return m_velocity;
}

void SquareRigidObject::SetVelocity(glm::vec2 newVelocity)
{
	m_velocity.x = newVelocity.x;
	m_velocity.y = newVelocity.y;
}

void SquareRigidObject::AddVelocity(glm::vec2 addedVelocity)
{
	m_velocity += addedVelocity;
}

void SquareRigidObject::PhysicsTick(float timeDelta)
{
	m_position += (m_velocity * timeDelta);
}
