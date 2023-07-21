#include "PhysicsEngine_ThreadWorker.h"

using namespace Physics::Engine;

PhysicsEngine_ThreadWorker::PhysicsEngine_ThreadWorker(PhysicsEngine* physicsEngine)
	: m_physicsEngine(physicsEngine)
	, m_thread(std::thread(&PhysicsEngine_ThreadWorker::WorkerThreadFunction, this))
	, m_mutex()
	, m_condition()
	, m_work()
{
}

void PhysicsEngine_ThreadWorker::AddWork(int xIndex, float timeDelta)
{
	PhysicsEngine_ThreadWorker::ResolveMoleculeCollisionWork* work = new PhysicsEngine_ThreadWorker::ResolveMoleculeCollisionWork();
	work->xIndex = xIndex;
	work->timeDelta = timeDelta;

	std::lock_guard<std::mutex> lock(m_mutex);
	m_work.push_back(work);
	m_condition.notify_one();
}

void PhysicsEngine_ThreadWorker::WorkerThreadFunction()
{
    while (true) {
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock);

		while (m_work.size() > 0)
		{
			const ResolveMoleculeCollisionWork* work = m_work.back();
			m_work.pop_back();
			lock.unlock();
			m_physicsEngine->ResolveMoleculeCollisions(work->xIndex, work->timeDelta);
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