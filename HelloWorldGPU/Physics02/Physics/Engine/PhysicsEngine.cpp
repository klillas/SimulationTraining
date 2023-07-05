#include "PhysicsEngine.h"
#include "./Physics/PhysicsConfiguration.h"
#include "Debug/DebugOutput.h"
#include "Physics/RigidObjects/GasMolecules.h"

using namespace Physics::Engine;

PhysicsEngine::PhysicsEngine()
	: m_gasMolecules({})
	, m_vertices({})
{
	
}

void PhysicsEngine::AddGasMolecule(GasMolecules::GasMolecule* rigidObject)
{
	m_gasMolecules.push_back(rigidObject);
}

void PhysicsEngine::PhysicsTick(float timeDelta)
{
	// Move objects according to their velocities
	for (unsigned i = 0; i < m_gasMolecules.size(); i++)
	{
		GasMolecules::PhysicsTick(timeDelta, m_gasMolecules[i]);

		ResolveMoleculeCollisions(timeDelta);
		ResolveWallCollisions(timeDelta);
	}
}

void PhysicsEngine::ResolveMoleculeCollisions(float timeDelta)
{
	const float epsilon = 0.001f;
	static const float moleculeRadius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;
	static const float moleculeMass = PhysicsConfiguration::GasMoleculeMass;

	// Resolve collisions
	for (unsigned i = 0; i < m_gasMolecules.size(); i++)
	{
		GasMolecules::GasMolecule* ro1 = m_gasMolecules[i];
		for (unsigned j = 0; j < m_gasMolecules.size(); j++)
		{
			GasMolecules::GasMolecule* ro2 = m_gasMolecules[j];
			if (i != j && GasMolecules::Intersects(ro1, ro2))
			{
				// Calculate the relative velocity
				glm::vec2 relativeVelocity = ro2->velocity - ro1->velocity;

				// Calculate the distance between the centers of the circles
				glm::vec2 displacement = ro2->position - ro1->position;
				float distance = glm::length(displacement);

				// Calculate the unit normal and tangential vectors
				glm::vec2 normal = displacement / distance;
				glm::vec2 tangential(-normal.y, normal.x);

				// Exchange the relativy velocity between the two molecules
				float velocityMagnitude = glm::dot(relativeVelocity, normal);
				glm::vec2 velocityAdjustment = velocityMagnitude * normal;
				ro1->velocity += velocityAdjustment;
				ro2->velocity -= velocityAdjustment;

				// Separate the circles to avoid overlapping
				float overlap = (moleculeRadius + moleculeRadius) - distance;
				glm::vec2 separation = (overlap * normal * 0.5f) + epsilon;
				ro1->position -= separation;
				ro2->position += separation;
			}
		}
	}
}

void PhysicsEngine::ResolveWallCollisions(float timeDelta)
{
	const float epsilon = 0.001f;
	static const float moleculeRadius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;

	// Resolve wall collisions
	for (unsigned i = 0; i < m_gasMolecules.size(); i++)
	{
		GasMolecules::GasMolecule* ro1 = m_gasMolecules[i];
		// TODO: This dynamic casting to IShape all over the place needs to stop
		glm::vec2 position = ro1->position;
		glm::vec2 velocity = ro1->velocity;

		if ((position.x + moleculeRadius) > 1.0f)
		{
			position.x = (1.0f - moleculeRadius - epsilon);
			ro1->position = position;
			if (velocity.x > 0)
			{
				velocity.x = -velocity.x;
				ro1->velocity = velocity;
			}
		}
		else if ((position.x - moleculeRadius) < -1.0f)
		{
			position.x = (-1.0f + moleculeRadius + epsilon);
			ro1->position = position;
			if (velocity.x < 0)
			{
				velocity.x = -velocity.x;
				ro1->velocity = velocity;
			}
		}

		if ((position.y + moleculeRadius) > 1.0f)
		{
			position.y = (1.0f - moleculeRadius - epsilon);
			ro1->position = position;
			if (velocity.y > 0)
			{
				velocity.y = -velocity.y;
				ro1->velocity = velocity;
			}
		}
		else if ((position.y - moleculeRadius) < -1.0f)
		{
			position.y = (-1.0f + moleculeRadius + epsilon);
			ro1->position = position;
			if (velocity.y < 0)
			{
				velocity.y = -velocity.y;
				ro1->velocity = velocity;
			}
		}
	}
}

std::vector<VulkanInit::Vertex> PhysicsEngine::GetVertices()
{
	m_vertices.clear();
	for (unsigned i = 0; i < m_gasMolecules.size(); i++)
	{
		GasMolecules::GetVertices(m_gasMolecules[i], &m_vertices);
	}

	return m_vertices;
}