#include "PhysicsEngine_ThreadWorker.h"

using namespace Physics::Engine;

PhysicsEngine_ThreadWorker::PhysicsEngine_ThreadWorker(PhysicsEngine* physicsEngine, SpaceGridMolecules* spaceGridMolecules)
	: m_physicsEngine(physicsEngine)
	, m_spaceGridMolecules(spaceGridMolecules)
	, m_thread(std::thread(&PhysicsEngine_ThreadWorker::WorkerThreadFunction, this))
	, m_mutex()
	, m_condition()
	, m_ResolveMoleculeCollisionWork()
	, m_ResolvePhysicsTickWork()
{

}

void PhysicsEngine_ThreadWorker::AddResolveMoleculeCollisionWork(int xIndex, float timeDelta)
{
	ResolveMoleculeCollisionWork* work = new ResolveMoleculeCollisionWork();
	work->xIndex = xIndex;
	work->timeDelta = timeDelta;

	std::lock_guard<std::mutex> lock(m_mutex);
	m_ResolveMoleculeCollisionWork.push_back(work);
	m_condition.notify_one();
}

void PhysicsEngine_ThreadWorker::AddResolvePhysicsTickWork(float timeDelta, std::vector<GasMolecules::GasMolecule*> gasMolecules, unsigned startIndex, unsigned lastIndex)
{
	ResolvePhysicsTickWork* work = new ResolvePhysicsTickWork();
	work->timeDelta = timeDelta;
	work->gasMolecules = gasMolecules;
	work->startIndex = startIndex;
	work->lastIndex = lastIndex;

	std::lock_guard<std::mutex> lock(m_mutex);
	m_ResolvePhysicsTickWork.push_back(work);
	m_condition.notify_one();
}

void PhysicsEngine_ThreadWorker::AddUpdateMoleculesCellLocation(std::vector<GasMolecules::GasMolecule*>& gasMolecules, unsigned startIndex, unsigned lastIndex)
{
	UpdateMoleculesCellLocationWork* work = new UpdateMoleculesCellLocationWork(gasMolecules, startIndex, lastIndex);

	std::lock_guard<std::mutex> lock(m_mutex);
	m_UpdateMoleculesCellLocationWork.push_back(work);
	m_condition.notify_one();
}

void PhysicsEngine_ThreadWorker::AddResetMoleculeCellLocationskWork(unsigned startIndex, unsigned lastIndex)
{
	ResetMoleculeCellLocationskWork* work = new ResetMoleculeCellLocationskWork();
	work->lastIndex = lastIndex;
	work->startIndex = startIndex;

	std::lock_guard<std::mutex> lock(m_mutex);
	m_ResetMoleculeCellLocationskWork.push_back(work);
	m_condition.notify_one();
}

void PhysicsEngine_ThreadWorker::AddResolveWallCollisionsWork(float timeDelta, unsigned moleculeIDStart, unsigned moleculeIDLast)
{
	ResolveWallCollisionsWork* work = new ResolveWallCollisionsWork();
	work->timeDelta = timeDelta;
	work->moleculeIDStart = moleculeIDStart;
	work->moleculeIDLast = moleculeIDLast;

	std::lock_guard<std::mutex> lock(m_mutex);
	m_ResolveWallCollisionsWork.push_back(work);
	m_condition.notify_one();
}

void PhysicsEngine_ThreadWorker::WorkerThreadFunction()
{
    while (true) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock);

		while (m_ResolveMoleculeCollisionWork.size() > 0)
		{
			const ResolveMoleculeCollisionWork* work = m_ResolveMoleculeCollisionWork.back();
			m_ResolveMoleculeCollisionWork.pop_back();
			lock.unlock();
			m_physicsEngine->ResolveMoleculeCollisions(work->xIndex, work->timeDelta);
			lock.lock();

			delete work;

			if (m_PhysicsEngine_ThreadWorker_WorkDoneCallback != nullptr)
			{
				m_PhysicsEngine_ThreadWorker_WorkDoneCallback();
			}
		}

		while (m_ResolvePhysicsTickWork.size() > 0)
		{
			const ResolvePhysicsTickWork* work = m_ResolvePhysicsTickWork.back();
			m_ResolvePhysicsTickWork.pop_back();
			lock.unlock();
			for (unsigned i = work->startIndex; i <= work->lastIndex; i++)
			{
				GasMolecules::PhysicsTick(work->timeDelta, work->gasMolecules[i]);
			}
			lock.lock();

			delete work;

			if (m_PhysicsEngine_ThreadWorker_WorkDoneCallback != nullptr)
			{
				m_PhysicsEngine_ThreadWorker_WorkDoneCallback();
			}
		}

		while (m_UpdateMoleculesCellLocationWork.size() > 0)
		{
			const UpdateMoleculesCellLocationWork* work = m_UpdateMoleculesCellLocationWork.back();
			m_UpdateMoleculesCellLocationWork.pop_back();
			lock.unlock();
			m_spaceGridMolecules->UpdateMoleculesCellLocation(work->gasMolecules, work->startColumnIndex, work->lastColumnIndex);
			lock.lock();

			delete work;

			if (m_PhysicsEngine_ThreadWorker_WorkDoneCallback != nullptr)
			{
				m_PhysicsEngine_ThreadWorker_WorkDoneCallback();
			}
		}

		while (m_ResetMoleculeCellLocationskWork.size() > 0)
		{
			const ResetMoleculeCellLocationskWork* work = m_ResetMoleculeCellLocationskWork.back();
			m_ResetMoleculeCellLocationskWork.pop_back();
			lock.unlock();
			m_spaceGridMolecules->ResetMoleculeCellLocations(work->startIndex, work->lastIndex);
			lock.lock();

			delete work;

			if (m_PhysicsEngine_ThreadWorker_WorkDoneCallback != nullptr)
			{
				m_PhysicsEngine_ThreadWorker_WorkDoneCallback();
			}
		}

		while (m_ResolveWallCollisionsWork.size() > 0)
		{
			const ResolveWallCollisionsWork* work = m_ResolveWallCollisionsWork.back();
			m_ResolveWallCollisionsWork.pop_back();
			lock.unlock();
			m_physicsEngine->ResolveWallCollisions(work->timeDelta, work->moleculeIDStart, work->moleculeIDLast);
			lock.lock();

			delete work;

			if (m_PhysicsEngine_ThreadWorker_WorkDoneCallback != nullptr)
			{
				m_PhysicsEngine_ThreadWorker_WorkDoneCallback();
			}
		}
    }
}

void PhysicsEngine_ThreadWorker::RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback callback)
{
	assert(callback);
	assert(!m_PhysicsEngine_ThreadWorker_WorkDoneCallback);

	m_PhysicsEngine_ThreadWorker_WorkDoneCallback = callback;
}