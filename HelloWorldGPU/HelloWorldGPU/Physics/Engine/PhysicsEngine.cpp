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
	for (unsigned i = 0; i < rigidObjects.size(); i++)
	{
		rigidObjects[i]->PhysicsTick(timeDelta);
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