#include "PhysicsEngine.h"
#include "./Physics/Shapes/IShape.h"
#include "./Physics/PhysicsConfiguration.h"
#include "Debug/DebugOutput.h"

using namespace Physics::Engine;
using namespace Physics::Shapes;

PhysicsEngine::PhysicsEngine()
	: m_gasMolecules({})
	, m_vertices({})
{
	
}

void PhysicsEngine::AddRigidObject(IRigidObject* rigidObject)
{
	m_gasMolecules.push_back(rigidObject);
}

void PhysicsEngine::PhysicsTick(float timeDelta)
{
	// Move objects according to their velocities
	for (unsigned i = 0; i < m_gasMolecules.size(); i++)
	{
		m_gasMolecules[i]->PhysicsTick(timeDelta);

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
		IRigidObject* ro1 = m_gasMolecules[i];
		// TODO: This dynamic casting to IShape all over the place needs to stop
		IShape* shape1 = (dynamic_cast<IShape*>(m_gasMolecules[i]));
		for (unsigned j = 0; j < m_gasMolecules.size(); j++)
		{
			IRigidObject* ro2 = m_gasMolecules[j];
			IShape* shape2 = (dynamic_cast<IShape*>(m_gasMolecules[j]));
			if (i != j && shape1->Intersects(shape2))
			{
				// Calculate the relative velocity
				glm::vec2 relativeVelocity = ro2->GetVelocity() - ro1->GetVelocity();

				// Calculate the distance between the centers of the circles
				glm::vec2 displacement = shape2->GetPosition() - shape1->GetPosition();
				float distance = glm::length(displacement);

				// Calculate the unit normal and tangential vectors
				glm::vec2 normal = displacement / distance;
				glm::vec2 tangential(-normal.y, normal.x);

				// Exchange the relativy velocity between the two molecules
				float velocityMagnitude = glm::dot(relativeVelocity, normal);
				glm::vec2 velocityAdjustment = velocityMagnitude * normal;
				ro1->AddVelocity(velocityAdjustment);
				ro2->AddVelocity(-velocityAdjustment);

				// Separate the circles to avoid overlapping
				float overlap = (moleculeRadius + moleculeRadius) - distance;
				glm::vec2 separation = (overlap * normal * 0.5f) + epsilon;
				shape1->AddPosition(-separation);
				shape2->AddPosition(separation);
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
		IRigidObject* ro1 = m_gasMolecules[i];
		// TODO: This dynamic casting to IShape all over the place needs to stop
		IShape* shape1 = (dynamic_cast<IShape*>(m_gasMolecules[i]));
		glm::vec2 position = shape1->GetPosition();
		glm::vec2 velocity = ro1->GetVelocity();

		if ((position.x + moleculeRadius) > 1.0f)
		{
			position.x = (1.0f - moleculeRadius - epsilon);
			shape1->SetPosition(position);
			if (velocity.x > 0)
			{
				velocity.x = -velocity.x;
				ro1->SetVelocity(velocity);
			}
		}
		else if ((position.x - moleculeRadius) < -1.0f)
		{
			position.x = (-1.0f + moleculeRadius + epsilon);
			shape1->SetPosition(position);
			if (velocity.x < 0)
			{
				velocity.x = -velocity.x;
				ro1->SetVelocity(velocity);
			}
		}

		if ((position.y + moleculeRadius) > 1.0f)
		{
			position.y = (1.0f - moleculeRadius - epsilon);
			shape1->SetPosition(position);
			if (velocity.y > 0)
			{
				velocity.y = -velocity.y;
				ro1->SetVelocity(velocity);
			}
		}
		else if ((position.y - moleculeRadius) < -1.0f)
		{
			position.y = (-1.0f + moleculeRadius + epsilon);
			shape1->SetPosition(position);
			if (velocity.y < 0)
			{
				velocity.y = -velocity.y;
				ro1->SetVelocity(velocity);
			}
		}
	}
}

std::vector<VulkanInit::Vertex> PhysicsEngine::GetVertices()
{
	m_vertices.clear();
	for (unsigned i = 0; i < m_gasMolecules.size(); i++)
	{
		std::vector<VulkanInit::Vertex> shapeVertices = (dynamic_cast<IShape*>(m_gasMolecules[i]))->GetVertices();
		m_vertices.insert(m_vertices.end(), shapeVertices.begin(), shapeVertices.end());
	}

	return m_vertices;
}