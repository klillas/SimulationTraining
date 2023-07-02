#include "GasMoleculeRigidObject.h"

using namespace Physics::RigidObjects;

GasMoleculeRigidObject::GasMoleculeRigidObject(glm::vec2 position, glm::vec2 velocity)
	: GasMolecule(position)
	, m_velocity(velocity)
{

}

GasMoleculeRigidObject::GasMoleculeRigidObject()
	: GasMolecule(),
	m_velocity(0.0f, 0.0f)
{

}

glm::vec2 GasMoleculeRigidObject::GetVelocity()
{
	return m_velocity;
}

void GasMoleculeRigidObject::SetVelocity(glm::vec2 newVelocity)
{
	m_velocity.x = newVelocity.x;
	m_velocity.y = newVelocity.y;
}

void GasMoleculeRigidObject::AddVelocity(glm::vec2 addedVelocity)
{
	m_velocity += addedVelocity;
}

void GasMoleculeRigidObject::PhysicsTick(float timeDelta)
{
	m_position += (m_velocity * timeDelta);
}
