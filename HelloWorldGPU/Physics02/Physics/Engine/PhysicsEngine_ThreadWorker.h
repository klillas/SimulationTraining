#pragma once
#include "PhysicsEngine.h"

#include <thread>
#include <mutex>

using namespace Physics::Engine;

namespace Physics::Engine
{
	class PhysicsEngine_ThreadWorker
	{
	public:
		typedef void(*PhysicsEngine_ThreadWorker_WorkDoneCallback)();

		struct ResolveMoleculeCollisionWork
		{
			unsigned xIndex;
			float timeDelta;
		};

		PhysicsEngine_ThreadWorker(PhysicsEngine* physicsEngine);

		void AddWork(int xIndex, float timeDelta);

		void RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback callback);

	private:
		void WorkerThreadFunction();

		PhysicsEngine* m_physicsEngine;
		std::thread m_thread;
		std::mutex m_mutex;
		std::condition_variable m_condition;
		std::vector<const ResolveMoleculeCollisionWork*> m_work;
		PhysicsEngine_ThreadWorker_WorkDoneCallback m_PhysicsEngine_ThreadWorker_WorkDoneCallback;
	};
};

