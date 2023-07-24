#pragma once

namespace Physics
{
	class PhysicsConfiguration
	{
	public:
		static inline const unsigned GasMoleculeStartCount = 226000;
		static inline const unsigned GasMoleculeMax = 1000000;

		static inline const glm::vec2 PhysicsEngineBottomLeft = { -1.0f, 1.0f };
		static inline const glm::vec2 PhysicsEngineTopRight = { 1.0f, -1.0f };
		static inline const float PhysicsEngineWidth = PhysicsEngineTopRight.x - PhysicsEngineBottomLeft.x;
		static inline const float PhysicsEngineHeight = PhysicsEngineBottomLeft.y - PhysicsEngineTopRight.y;
		/// <summary>
		/// We currently map physics coordinate directly to screen coordinates, we need to know the limit negative positions
		/// for calculations where we want to offset and calculate with only >= 0 coordinates
		/// </summary>
		static inline const glm::vec2 PhysicsEngineStartPosNegativeDelta = { 1.0f, 1.0f };
		static inline const unsigned PhysicsEngineFilterMoleculeFactor = 1;
		static inline const unsigned PhysicsEngineWorkerThreads = 8;
		/// <summary>
		/// The width of cell columns which divide parallel thread work
		/// If 3 threads and delta of 4, the first batch of work will be thread1: column 0, thread2: column 4, thread3: column 8
		/// </summary>
		static inline const unsigned PhysicsEngineWorkerThreadsCellColumnDelta = 5;

		static inline const float GasMoleculeDiameter = 0.003f;
		static inline const float GasMoleculeMass = 0.01f;

		static inline const unsigned SpaceGridMoleculesWidth = 400;
		static inline const unsigned SpaceGridMoleculesHeight = 400;

	private:
		// Right now we don't support uneven division of cells / worker threads
		static_assert((SpaceGridMoleculesWidth % PhysicsEngineWorkerThreads) == 0);
		static_assert((SpaceGridMoleculesHeight % PhysicsEngineWorkerThreads) == 0);
		// Due to parallelization issues of the molecule cell assignment update we need to work on cells that are 
		static_assert(SpaceGridMoleculesWidth % (PhysicsEngineWorkerThreads * PhysicsEngineWorkerThreadsCellColumnDelta) == 0);
	};
};