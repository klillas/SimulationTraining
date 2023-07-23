#include "PhysicsEngine_ThreadWorker.h"

using namespace Physics::Engine;

PhysicsEngine_ThreadWorker::PhysicsEngine_ThreadWorker(PhysicsEngine* physicsEngine)
	: m_physicsEngine(physicsEngine)
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
    }
}

void PhysicsEngine_ThreadWorker::RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback callback)
{
	assert(callback);
	assert(!m_PhysicsEngine_ThreadWorker_WorkDoneCallback);

	m_PhysicsEngine_ThreadWorker_WorkDoneCallback = callback;
}