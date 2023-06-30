#include "PhysicsEngine.h"
#include "./Physics/Shapes/IShape.h"

using namespace Physics::Engine;
using namespace Physics::Shapes;

PhysicsEngine::PhysicsEngine()
	: rigidObjects({})
	, m_vertices({})
{
	
}

void PhysicsEngine::AddRigidObject(IRigidObject* rigidObject)
{
	rigidObjects.push_back(rigidObject);
}

void PhysicsEngine::PhysicsTick(float timeDelta)
{
	// Move objects according to their velocities
	for (unsigned i = 0; i < rigidObjects.size(); i++)
	{
		rigidObjects[i]->PhysicsTick(timeDelta);

		TickCollisionDetection(timeDelta);
	}
}

void PhysicsEngine::TickCollisionDetection(float timeDelta)
{
	glm::vec2 first{-1.0f, 1.0f};
	glm::vec2 second{2.0f, 3.0f};
	glm::vec2 result = first - second;
	glm::vec2 resultTwo = second + first;
	glm::vec2 absOne = glm::abs(result);
	glm::vec2 absTwo = glm::abs(resultTwo);

	const float epsilon = 0.001f;
	// Resolve collisions
	for (unsigned i = 0; i < rigidObjects.size(); i++)
	{
		IRigidObject* ro1 = rigidObjects[i];
		// TODO: This dynamic casting to IShape all over the place needs to stop
		IShape* shape1 = (dynamic_cast<IShape*>(rigidObjects[i]));
		for (unsigned j = 0; j < rigidObjects.size(); j++)
		{
			IRigidObject* ro2 = rigidObjects[j];
			IShape* shape2 = (dynamic_cast<IShape*>(rigidObjects[j]));
			if (i != j && shape1->Intersects(shape2))
			{
				// Calculate the relative velocity
				glm::vec2 relativeVelocity = glm::abs(ro2->GetVelocity() - ro1->GetVelocity());

				// Calculate the distance between the centers of the circles
				glm::vec2 displacement = shape2->GetPosition() - shape1->GetPosition();
				float distance = glm::length(displacement);

				// Calculate the unit normal and tangential vectors
				glm::vec2 normal = displacement / distance;
				glm::vec2 tangential(-normal.y, normal.x);

				// Calculate the normal and tangential components of the relative velocity
				float relativeVelocityNormal = glm::dot(relativeVelocity, normal);
				float relativeVelocityTangential = glm::dot(relativeVelocity, tangential);

				// Calculate the impulse (change in momentum) along the normal direction
				float impulseMagnitude = (-(1 + epsilon) * relativeVelocityNormal) /
					(1 / ro1->GetMass() + 1 / ro2->GetMass());

				// Apply the impulse to update the velocities
				ro1->AddVelocity((impulseMagnitude / ro1->GetMass()) * normal);
				ro2->AddVelocity(-(impulseMagnitude / ro2->GetMass()) * normal);

				// Apply friction to update the tangential component of the velocities
				/*
				const float frictionCoefficient = 0.8f;
				glm::vec2 frictionImpulse = -(frictionCoefficient * relativeVelocityTangential) * tangential;
				circle1.velocity += (frictionImpulse / circle1.mass);
				circle2.velocity += (frictionImpulse / circle2.mass);
				*/

				// Separate the circles to avoid overlapping
				float shape1Radius = (shape1->GetSize() / 2);
				float shape2Radius = (shape2->GetSize() / 2);
				float overlap = (shape1Radius + shape2Radius) - distance;
				glm::vec2 separation = (overlap * normal * 0.5f) + epsilon;
				shape1->AddPosition(-separation);
				shape2->AddPosition(separation);
			}
		}
	}

	// Resolve wall collisions
	for (unsigned i = 0; i < rigidObjects.size(); i++)
	{
		IRigidObject* ro1 = rigidObjects[i];
		// TODO: This dynamic casting to IShape all over the place needs to stop
		IShape* shape1 = (dynamic_cast<IShape*>(rigidObjects[i]));
		glm::vec2 position = shape1->GetPosition();
		glm::vec2 velocity = ro1->GetVelocity();
		float radius = (shape1->GetSize() / 2);

		if ((position.x + radius) > 1.0f)
		{
			position.x = (position.x - radius - epsilon);
			shape1->SetPosition(position);
			if (velocity.x > 0)
			{
				velocity.x = -velocity.x;
				ro1->SetVelocity(velocity);
			}
		}
		else if ((position.x - radius) < -1.0f)
		{
			position.x = (-1.0f + radius + epsilon);
			shape1->SetPosition(position);
			if (velocity.x < 0)
			{
				velocity.x = -velocity.x;
				ro1->SetVelocity(velocity);
			}
		}
		
		if ((position.y + radius) > 1.0f)
		{
			position.y = (1.0f - radius - epsilon);
			shape1->SetPosition(position);
			if (velocity.y > 0)
			{
				velocity.y = -velocity.y;
				ro1->SetVelocity(velocity);
			}
		}
		else if ((position.y - radius) < -1.0f)
		{
			position.y = (-1.0f + radius + epsilon);
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
	for (unsigned i = 0; i < rigidObjects.size(); i++)
	{
		std::vector<VulkanInit::Vertex> shapeVertices = (dynamic_cast<IShape*>(rigidObjects[i]))->GetVertices();
		m_vertices.insert(m_vertices.end(), shapeVertices.begin(), shapeVertices.end());
	}

	return m_vertices;
}