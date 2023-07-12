#pragma once

#include "./Physics/RigidObjects/GasMolecules.h"
#include <glm/glm.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "Physics/PhysicsConfiguration.h"

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
		};

		SpaceGridMolecules();

		void AddGasMolecule(GasMolecules::GasMolecule* newMolecule);

		void UpdateGasMoleculeCells();

		void UpdateMoleculeCellLocation(GasMolecules::GasMolecule* molecule);

		Cell* GetCell(unsigned indexX, unsigned indexY);

	private:
		GasMolecules::GasMolecule m_startEndMolecules[PhysicsConfiguration::SpaceGridMoleculesWidth + 2][PhysicsConfiguration::SpaceGridMoleculesHeight + 2][2];

		/// <summary>
		/// Cell grid containing all the cells for the space partitioning
		/// There is an additional cell on the top, bottom, left and right side which will always be empty (outside the bounds of the simulation)
		/// This will allow us to avoid a bunch of boundary check conditions inside the heavy duty update code
		/// </summary>
		Cell m_cellGrid[PhysicsConfiguration::SpaceGridMoleculesWidth + 2][PhysicsConfiguration::SpaceGridMoleculesHeight + 2];
	};
};

