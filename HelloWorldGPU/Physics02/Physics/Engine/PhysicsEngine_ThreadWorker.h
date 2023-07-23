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

		struct ResolvePhysicsTickWork
		{
			float timeDelta;
			unsigned subticks;
			std::vector<GasMolecules::GasMolecule*> gasMolecules;
			unsigned startIndex;
			unsigned lastIndex;
		};

		PhysicsEngine_ThreadWorker(PhysicsEngine* physicsEngine);

		void AddResolveMoleculeCollisionWork(int xIndex, float timeDelta);

		void AddResolvePhysicsTickWork(float timeDelta, unsigned subticks, std::vector<GasMolecules::GasMolecule*> gasMolecules, unsigned startIndex, unsigned lastIndex);

		void RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback callback);

	private:
		void WorkerThreadFunction();

		PhysicsEngine* m_physicsEngine;
		std::thread m_thread;
		std::mutex m_mutex;
		std::condition_variable m_condition;
		std::vector<const ResolveMoleculeCollisionWork*> m_ResolveMoleculeCollisionWork;
		std::vector<const ResolvePhysicsTickWork*> m_ResolvePhysicsTickWork;

		PhysicsEngine_ThreadWorker_WorkDoneCallback m_PhysicsEngine_ThreadWorker_WorkDoneCallback;
	};
};

