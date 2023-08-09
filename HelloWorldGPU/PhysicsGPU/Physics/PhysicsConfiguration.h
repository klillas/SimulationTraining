#pragma once

namespace Physics
{
	class PhysicsConfiguration
	{
	public:
		static inline const unsigned GasMoleculeStartCount = 288000;
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

		static inline const float GasMoleculeDiameter = 0.003f;
		static inline const float GasMoleculeMass = 0.01f;

		static inline const unsigned SpaceGridCellMoleculesWidth = 400;
		static inline const unsigned SpaceGridCellMoleculesHeight = 400;
	};
};