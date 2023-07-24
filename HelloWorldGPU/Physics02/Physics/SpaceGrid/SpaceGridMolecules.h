#pragma once

#include "./Physics/RigidObjects/GasMolecules.h"
#include <glm/glm.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "Physics/PhysicsConfiguration.h"
#include <mutex>

using namespace Physics::RigidObjects;
using namespace Physics;

namespace Physics::SpaceGrid
{
	class SpaceGridMolecules
	{
	public:
		const unsigned CellCountX = PhysicsConfiguration::SpaceGridMoleculesWidth;
		const unsigned CellCountY = PhysicsConfiguration::SpaceGridMoleculesHeight;
		const float CellWidth = PhysicsConfiguration::PhysicsEngineWidth / PhysicsConfiguration::SpaceGridMoleculesWidth;
		const float CellHeight = PhysicsConfiguration::PhysicsEngineHeight / PhysicsConfiguration::SpaceGridMoleculesHeight;

		struct Cell
		{
		public:
			GasMolecules::GasMolecule* StartMolecule = nullptr;
			GasMolecules::GasMolecule* StopMolecule = nullptr;
			// Amount of molecules in this cell
			unsigned MoleculeCount = 0;
			std::mutex CellMutex;
		};

		SpaceGridMolecules();

		void AddGasMolecule(GasMolecules::GasMolecule* newMolecule);

		void UpdateGasMoleculeCells();

		void UpdateMoleculeCellLocation(GasMolecules::GasMolecule* molecule);

		Cell* GetCell(unsigned indexX, unsigned indexY);

		void RemoveGasMolecule(GasMolecules::GasMolecule* molecule);

		/// <summary>
		/// Temporarily hides a gas molecule in a cell. Can be shown again in the cell with ShowGasMolecule
		/// </summary>
		/// <param name="molecule"></param>
		void HideGasMolecule(GasMolecules::GasMolecule* molecule);

		/// <summary>
		/// Shows a hidden gas molecule
		/// </summary>
		/// <param name="molecule"></param>
		void ShowGasMolecule(GasMolecules::GasMolecule* molecule);

	private:
		GasMolecules::GasMolecule m_startEndMolecules[PhysicsConfiguration::SpaceGridMoleculesWidth + 2][PhysicsConfiguration::SpaceGridMoleculesHeight + 2][2];

		/// <summary>
		/// Cell grid containing all the cells for the space partitioning
		/// There is an additional cell on the top, bottom, left and right side which will always be empty (outside the bounds of the simulation)
		/// This will allow us to avoid a bunch of boundary check conditions inside the heavy duty update code
		/// </summary>
		Cell m_cellGrid[PhysicsConfiguration::SpaceGridMoleculesWidth + 2][PhysicsConfiguration::SpaceGridMoleculesHeight + 2];

		/// <summary>
		/// Can be used to perform a longer set of operations, like mutex locking two cells, without being afraid of race conditions between the two locks
		/// 1. Lock this mutex
		/// 2. Lock the first cell
		/// 3. Lock the second cell
		/// 4. Unlock this mutex
		/// 5. Operate on the cells
		/// 6. Unlock the first cell
		/// 7. Unlock the second cell
		/// </summary>
		std::mutex m_atomicOperationMutex;
	};
};

