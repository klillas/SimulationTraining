#pragma once
#include "PhysicsEngine.h"
#include "./Physics/SpaceGrid/SpaceGridMolecules.h"

#include <thread>
#include <mutex>

using namespace Physics::Engine;
using namespace Physics::SpaceGrid;

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
			std::vector<GasMolecules::GasMolecule*> gasMolecules;
			unsigned startIndex;
			unsigned lastIndex;
		};

		struct ResetMoleculeCellLocationskWork
		{
			unsigned startIndex;
			unsigned lastIndex;
		};

		struct UpdateMoleculesCellLocationWork
		{
			std::vector<GasMolecules::GasMolecule*>& gasMolecules;
			unsigned startColumnIndex;
			unsigned lastColumnIndex;

			UpdateMoleculesCellLocationWork(std::vector<GasMolecules::GasMolecule*>& molecules, unsigned start, unsigned last)
				: gasMolecules(molecules), startColumnIndex(start), lastColumnIndex(last)
			{
			}
		};

		struct ResolveWallCollisionsWork
		{
			float timeDelta;
			unsigned moleculeIDStart;
			unsigned moleculeIDLast;
		};

		PhysicsEngine_ThreadWorker(PhysicsEngine* physicsEngine, SpaceGridMolecules* spaceGridMolecules);

		void AddResolveMoleculeCollisionWork(int xIndex, float timeDelta);

		void AddResolvePhysicsTickWork(float timeDelta, std::vector<GasMolecules::GasMolecule*> gasMolecules, unsigned startIndex, unsigned lastIndex);

		void AddResetMoleculeCellLocationskWork(unsigned startIndex, unsigned lastIndex);

		void AddUpdateMoleculesCellLocation(std::vector<GasMolecules::GasMolecule*>& gasMolecules, unsigned startMoleculeIndex, unsigned lastMoleculeIndex);

		void AddResolveWallCollisionsWork(float timeDelta, unsigned moleculeIDStart, unsigned moleculeIDLast);

		void RegisterPhysicsEngine_ThreadWorker_WorkDoneCallback(PhysicsEngine_ThreadWorker_WorkDoneCallback callback);

	private:
		void WorkerThreadFunction();

		PhysicsEngine* m_physicsEngine;
		SpaceGridMolecules* m_spaceGridMolecules;
		std::thread m_thread;
		std::mutex m_mutex;
		std::condition_variable m_condition;
		std::vector<const ResolveMoleculeCollisionWork*> m_ResolveMoleculeCollisionWork;
		std::vector<const ResolvePhysicsTickWork*> m_ResolvePhysicsTickWork;
		std::vector<const ResetMoleculeCellLocationskWork*> m_ResetMoleculeCellLocationskWork;
		std::vector<const UpdateMoleculesCellLocationWork*> m_UpdateMoleculesCellLocationWork;
		std::vector<const ResolveWallCollisionsWork*> m_ResolveWallCollisionsWork;

		PhysicsEngine_ThreadWorker_WorkDoneCallback m_PhysicsEngine_ThreadWorker_WorkDoneCallback;
	};
};

