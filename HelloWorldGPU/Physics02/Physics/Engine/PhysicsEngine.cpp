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
	m_grid.AddGasMolecule(rigidObject);
}

unsigned PhysicsEngine::GasMoleculeCount()
{
	return m_gasMolecules.size();
}

void PhysicsEngine::PhysicsTick(float timeDelta)
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
			for (unsigned moleculeID = 1; moleculeID < moleculeCount; moleculeID++)
			{
				molecule = molecule->nextItem;
				// Temporarily remove molecule from cell, saves us an if check when going through all molecules in cell
				molecule->previousItem->nextItem = molecule->nextItem;
				molecule->nextItem->previousItem = molecule->previousItem;

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
				molecule->previousItem->nextItem = molecule;
				molecule->nextItem->previousItem = molecule;
			}
		}
	}
	
	// TODO: Implement boundary cell checking
}

void PhysicsEngine::ResolveMoleculeCollisions(GasMolecules::GasMolecule* molecule, SpaceGridMolecules::Cell* cell)
{
	const float epsilon = 0.001f;
	const float moleculeRadius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;
	const float moleculeMass = PhysicsConfiguration::GasMoleculeMass;

	GasMolecules::GasMolecule* moleculeTwo = cell->StartMolecule;
	for (unsigned cellPosition = 0; cellPosition < cell->MoleculeCount; cellPosition++)
	{
		moleculeTwo = moleculeTwo->nextItem;

		if (GasMolecules::Intersects(molecule, moleculeTwo))
		{
			// Calculate the relative velocity
			glm::vec2 relativeVelocity = moleculeTwo->velocity - molecule->velocity;

			// Calculate the distance between the centers of the circles
			glm::vec2 displacement = moleculeTwo->position - molecule->position;
			float distance = glm::length(displacement);

			// Calculate the unit normal and tangential vectors
			glm::vec2 normal = displacement / distance;
			glm::vec2 tangential(-normal.y, normal.x);

			// Exchange the relativy velocity between the two molecules
			float velocityMagnitude = glm::dot(relativeVelocity, normal);
			glm::vec2 velocityAdjustment = velocityMagnitude * normal;
			molecule->velocity += velocityAdjustment;
			moleculeTwo->velocity -= velocityAdjustment;

			// Separate the circles to avoid overlapping
			float overlap = (moleculeRadius + moleculeRadius) - distance;
			glm::vec2 separation = (overlap * normal * 0.5f) + epsilon;
			molecule->position -= separation;
			moleculeTwo->position += separation;
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

void PhysicsEngine::UpdateMoleculeCellPositions()
{
	// Reset all cell contents
	/*
	for (unsigned xIndex = 0; xIndex < m_grid.CellCountX; xIndex++)
	{
		for (unsigned yIndex = 0; yIndex < m_grid.CellCountY; yIndex++)
		{
			m_grid.m_cellGrid[xIndex][yIndex].MoleculeCount = 0;
			m_grid.m_cellGrid[xIndex][yIndex].StartMolecule->nextItem = m_grid.m_cellGrid[xIndex][yIndex].StopMolecule;
			m_grid.m_cellGrid[xIndex][yIndex].StopMolecule->previousItem = m_grid.m_cellGrid[xIndex][yIndex].StartMolecule;
		}
	}
	*/

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