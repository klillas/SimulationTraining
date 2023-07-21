#include "PhysicsEngine.h"
#include "./Physics/PhysicsConfiguration.h"
#include "Debug/DebugOutput.h"
#include "Physics/RigidObjects/GasMolecules.h"
#include <cmath>
#include <thread>
#include <mutex>
#include "PhysicsEngine_ThreadWorker.h"

// TODO: Make multi-threading configurable via setting

// TODO: Collisions are calculated on multiple threads on a column basis. Columns are added to work queues one by one. 
//       The simulation is not deterministic because columns have race conditions on when they are calculated, and the 
//       molecule states can be calculated in different order.
//       A deterministic physics step calculation can be done if we calculate every x column first, wait until done, then calculate every x+1 column and so on.

using namespace Physics::Engine;

unsigned worksPending = 0;
std::mutex workStatusMutex;
std::condition_variable condition;

void PhysicsEngine::PhysicsEngine_ThreadWorker_WorkDoneCallback()
{
	std::lock_guard<std::mutex> lock(workStatusMutex);
	worksPending--;

	if (worksPending == 0)
	{
		condition.notify_one();
	}
}

PhysicsEngine::PhysicsEngine()
	: m_gasMolecules({})
	, m_vertices({})
	, m_grid()
	, m_worker1(new PhysicsEngine_ThreadWorker(this))
	, m_worker2(new PhysicsEngine_ThreadWorker(this))
	, m_worker3(new PhysicsEngine_ThreadWorker(this))
	, m_worker4(new PhysicsEngine_ThreadWorker(this))
	, m_worker5(new PhysicsEngine_ThreadWorker(this))
	, m_worker6(new PhysicsEngine_ThreadWorker(this))
	, m_worker7(new PhysicsEngine_ThreadWorker(this))
	, m_worker8(new PhysicsEngine_ThreadWorker(this))
{
	m_worker1->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
	m_worker2->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
	m_worker3->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
	m_worker4->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
	m_worker5->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
	m_worker6->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
	m_worker7->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
	m_worker8->RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback);
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
		std::unique_lock<std::mutex> lock(workStatusMutex);
		condition.wait(lock);

		ResolveWallCollisions(timeDelta);
		UpdateMoleculeCellPositions();
	}
}

void PhysicsEngine::ResolveMoleculeCollisions(unsigned xIndex, float timeDelta)
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
			//m_grid.HideGasMolecule(molecule);

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
			//m_grid.ShowGasMolecule(molecule);
		}
	}
}

//static void TestMe();

void PhysicsEngine::ResolveMoleculeCollisions(float timeDelta)
{
	assert(m_grid.CellCountX % 8 == 0);
	for (unsigned xIndex = 0; xIndex < m_grid.CellCountX; xIndex = xIndex + 8)
	{
		{
			std::lock_guard<std::mutex> lock(workStatusMutex);
			worksPending = worksPending + 8;
		}
		m_worker1->AddWork(xIndex, timeDelta);
		m_worker2->AddWork(xIndex + 1, timeDelta);
		m_worker3->AddWork(xIndex + 2, timeDelta);
		m_worker4->AddWork(xIndex + 3, timeDelta);
		m_worker5->AddWork(xIndex + 4, timeDelta);
		m_worker6->AddWork(xIndex + 5, timeDelta);
		m_worker7->AddWork(xIndex + 6, timeDelta);
		m_worker8->AddWork(xIndex + 7, timeDelta);

		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex, timeDelta, this); });
		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex + 1, timeDelta, this); });
		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex + 2, timeDelta, this); });
		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex + 3, timeDelta, this); });
		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex + 4, timeDelta, this); });
		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex + 5, timeDelta, this); });
		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex + 6, timeDelta, this); });
		//threads.emplace_back([this, xIndex, timeDelta] { Thread_ResolveMoleculeCollisions(xIndex + 7, timeDelta, this); });

		// Wait for all threads to finish their work
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

		if (molecule != moleculeTwo && GasMolecules::Intersects(molecule, moleculeTwo))
		{
			// Move the molecules so they do not intersect anymore
			glm::vec2 collisionVector = molecule->position - moleculeTwo->position;
			glm::vec2 collisionNormalVector = glm::normalize(collisionVector);
			float collisionLength = glm::length(collisionVector);
			float intersectDepth = moleculesRadii - collisionLength;
			glm::vec2 moveDeltaVector = collisionNormalVector * intersectDepth;
			glm::vec2 moveDeltaHalfVector = moveDeltaVector / 2.0f;
			molecule->position += moveDeltaHalfVector;
			moleculeTwo->position -= moveDeltaHalfVector;

			// Calculate the collision velocity transfer
			glm::vec2 relativeVelocity = moleculeTwo->velocity - molecule->velocity;
			glm::vec2 collisionTangentVector(collisionVector.y, -collisionVector.x);
			glm::vec2 collisionTangentNormalVector = glm::normalize(collisionTangentVector);
			float velocityLength = glm::dot(relativeVelocity, collisionTangentNormalVector);
			glm::vec2 velocityOnTangent = collisionTangentNormalVector * velocityLength;
			glm::vec2 velocityPerpendicularToTangent = relativeVelocity - velocityOnTangent;

			molecule->velocity += velocityPerpendicularToTangent;
			moleculeTwo->velocity -= velocityPerpendicularToTangent;
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

void PhysicsEngine::GetVertices(std::vector<VulkanInit::Vertex>* verticesBuffer)
{
	verticesBuffer->clear();

	for (unsigned i = 0; i < m_gasMolecules.size(); i = i + PhysicsConfiguration::PhysicsEngineFilterMoleculeFactor)
	{
		GasMolecules::GetVertices(m_gasMolecules[i], verticesBuffer);
	}
}