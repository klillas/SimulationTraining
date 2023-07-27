#pragma once

#include "./Physics/RigidObjects/GasMolecules.h"
#include <glm/glm.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "Physics/PhysicsConfiguration.h"
#include <mutex>
#include <tbb/concurrent_vector.h>

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
			tbb::concurrent_vector<unsigned> MoleculeIDs;
		};

		SpaceGridMolecules();

		void AddGasMolecule(GasMolecules::GasMolecule* newMolecule, unsigned moleculeID);

		void UpdateGasMoleculeCells();

		void UpdateMoleculesCellLocation(std::vector<GasMolecules::GasMolecule*>& molecules);

		Cell* GetCell(unsigned indexX, unsigned indexY);

		void RemoveGasMolecule(GasMolecules::GasMolecule* molecule, unsigned moleculeID);

	private:
		/// <summary>
		/// Cell grid containing all the cells for the space partitioning
		/// There is an additional cell on the top, bottom, left and right side which will always be empty (outside the bounds of the simulation)
		/// This will allow us to avoid a bunch of boundary check conditions inside the heavy duty update code
		/// </summary>
		Cell m_cellGrid[PhysicsConfiguration::SpaceGridMoleculesWidth + 2][PhysicsConfiguration::SpaceGridMoleculesHeight + 2];
	};
};

