#include "PhysicsEngine.h"
#include "./Physics/PhysicsConfiguration.h"
#include "Debug/DebugOutput.h"
#include "Physics/RigidObjects/GasMolecules.h"
#include <cmath>

using namespace Physics::Engine;

PhysicsEngine::PhysicsEngine()
	: m_gasMolecules({})
	, m_vertices({})
{
	
}

void PhysicsEngine::AddGasMolecule(GasMolecules::GasMolecule* rigidObject)
{
	m_gasMolecules.push_back(rigidObject);
	m_grid.AddGasMolecule(rigidObject);
}

unsigned PhysicsEngine::GasMoleculeCount()
{
	return m_gasMolecules.size();
}

void PhysicsEngine::PhysicsTick(float timeDelta, unsigned subticks)
{
	timeDelta = timeDelta / subticks;
	for (unsigned i = 0; i < subticks; i++)
	{
		// Move objects according to their velocities
		for (unsigned i = 0; i < m_gasMolecules.size(); i++)
		{
			GasMolecules::PhysicsTick(timeDelta, m_gasMolecules[i]);
		}

		ResolveMoleculeCollisions(timeDelta);
		ResolveWallCollisions(timeDelta);
		UpdateMoleculeCellPositions();
	}
}

void PhysicsEngine::ResolveMoleculeCollisions(float timeDelta)
{
	for (unsigned xIndex = 0; xIndex < m_grid.CellCountX; xIndex++)
	{
		for (unsigned yIndex = 0; yIndex < m_grid.CellCountY; yIndex++)
		{
			SpaceGridMolecules::Cell* cell = m_grid.GetCell(xIndex, yIndex);
			unsigned moleculeCount = cell->MoleculeCount;
			// The first molecule is just an empty start molecule, we can safely skip any calculation on that
			GasMolecules::GasMolecule* molecule = cell->StartMolecule;
			for (unsigned moleculeID = 0; moleculeID < moleculeCount; moleculeID++)
			{
				molecule = molecule->nextItem;
				// Temporarily remove molecule from cell, saves us an if check when going through all molecules in cell
				m_grid.HideGasMolecule(molecule);

				// Check collisions with molecules in this cell, and all connected cells
				// Assume that cell size is at least as large as the radius+radius of the two molecules
				// If molecule size becomes larger, we would need to check even more cells...
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex - 1, yIndex - 1));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex + 0, yIndex - 1));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex + 1, yIndex - 1));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex - 1, yIndex + 0));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex + 0, yIndex + 0));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex + 1, yIndex + 0));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex - 1, yIndex + 1));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex + 0, yIndex + 1));
				ResolveMoleculeCollisions(molecule, m_grid.GetCell(xIndex + 1, yIndex + 1));

				// Place molecule back into the cell
				m_grid.ShowGasMolecule(molecule);
			}
		}
	}
}

void PhysicsEngine::ResolveMoleculeCollisions(GasMolecules::GasMolecule* molecule, SpaceGridMolecules::Cell* cell)
{
	const float epsilon = 0.001f;
	const float moleculeRadius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;
	const float moleculesRadii = moleculeRadius * 2;
	const float moleculeMass = PhysicsConfiguration::GasMoleculeMass;

	GasMolecules::GasMolecule* moleculeTwo = cell->StartMolecule;
	for (unsigned cellPosition = 0; cellPosition < cell->MoleculeCount; cellPosition++)
	{
		moleculeTwo = moleculeTwo->nextItem;

		// Move the molecules so they do not intersect anymore
		glm::vec2 collisionVector = molecule->position - moleculeTwo->position;
		
		//float collisionLength = glm::length(collisionVector);
		float collisionLength = 1.0f / Q_rsqrt((collisionVector.x * collisionVector.x)+(collisionVector.y * collisionVector.y));;

		//glm::vec2 collisionNormalVector = glm::normalize(collisionVector);
		glm::vec2 collisionNormalVector(collisionVector.x / collisionLength, collisionVector.y / collisionLength);
		
		float intersectDepth = moleculesRadii - collisionLength;

		// intersectDepth is set to 0 if it is negative (meaning molecules do not intersect)
		float intersectDepthAbs = intersectDepth;
		unsigned* farrPtr = (unsigned*) & intersectDepthAbs;
		farrPtr[0] &= 0b01111111111111111111111111111111;
		intersectDepth = (intersectDepth + intersectDepthAbs) / 2.0f;

		glm::vec2 moveDeltaVector = collisionNormalVector * intersectDepth;
		glm::vec2 moveDeltaHalfVector = moveDeltaVector / 2.0f;
		molecule->position += moveDeltaHalfVector;
		moleculeTwo->position -= moveDeltaHalfVector;
			
		// Calculate the collision velocity transfer
		glm::vec2 relativeVelocity = (moleculeTwo->velocity - molecule->velocity);
		relativeVelocity = relativeVelocity * intersectDepth / (intersectDepth + FLT_MIN);
		glm::vec2 collisionTangentVector(collisionVector.y, -collisionVector.x);

		//glm::vec2 collisionTangentNormalVector = glm::normalize(collisionTangentVector);
		float collisionTangentVectorLength = 1.0f / Q_rsqrt((collisionTangentVector.x * collisionTangentVector.x) + (collisionTangentVector.y * collisionTangentVector.y));;
		glm::vec2 collisionTangentNormalVector(collisionTangentVector.x / collisionTangentVectorLength, collisionTangentVector.y / collisionTangentVectorLength);

		//float velocityMagnitude = glm::dot(relativeVelocity, collisionTangentNormalVector);
		float velocityMagnitude = (relativeVelocity.x * collisionTangentNormalVector.x) + (relativeVelocity.y * collisionTangentNormalVector.y);

		glm::vec2 velocityOnTangent = collisionTangentNormalVector * velocityMagnitude;
		glm::vec2 velocityPerpendicularToTangent = relativeVelocity - velocityOnTangent;

		molecule->velocity += velocityPerpendicularToTangent;
		moleculeTwo->velocity -= velocityPerpendicularToTangent;
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

void PhysicsEngine::UpdateMoleculeCellPositions()
{
	for (unsigned i = 0; i < m_gasMolecules.size(); i++)
	{
		m_grid.UpdateMoleculeCellLocation(m_gasMolecules[i]);
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

float PhysicsEngine::Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;							// evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);				// what the fuck?
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));	// 1st iteration
	//y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	y = y * 0.97f;	// Hack to move the error to the correct side

	return y;
}

