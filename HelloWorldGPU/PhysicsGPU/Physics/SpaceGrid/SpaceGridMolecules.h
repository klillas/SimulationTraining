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
		const unsigned CellCountX = PhysicsConfiguration::SpaceGridCellMoleculesWidth;
		const unsigned CellCountY = PhysicsConfiguration::SpaceGridCellMoleculesHeight;
		const float CellWidth = PhysicsConfiguration::PhysicsEngineWidth / PhysicsConfiguration::SpaceGridCellMoleculesWidth;
		const float CellHeight = PhysicsConfiguration::PhysicsEngineHeight / PhysicsConfiguration::SpaceGridCellMoleculesHeight;

		struct Cell
		{
			tbb::concurrent_vector<unsigned> MoleculeIDs;
		};

		SpaceGridMolecules();

		void AddGasMolecule(GasMolecules::GasMolecule* newMolecule, unsigned moleculeID);

		// Resets the list of cell molecule ID vectors in preparation to run UpdateMoleculesCellLocation
		// Thread safe
		void ResetMoleculeCellLocations(unsigned columnStartID, unsigned columnLastID);

		/// <summary>
		/// Updates molecule cell locations
		/// ResetMoleculeCellLocations() must be called first
		/// Thread safe
		/// </summary>
		/// <param name="molecules"></param>
		/// <param name="startIndex"></param>
		/// <param name="lastIndex"></param>
		void UpdateMoleculesCellLocation(std::vector<GasMolecules::GasMolecule*>& molecules, unsigned startIndex, unsigned lastIndex);

		Cell* GetCell(unsigned indexX, unsigned indexY);

		void RemoveGasMolecule(GasMolecules::GasMolecule* molecule, unsigned moleculeID);

	private:
		/// <summary>
		/// Cell grid containing all the cells for the space partitioning
		/// There is an additional cell on the top, bottom, left and right side which will always be empty (outside the bounds of the simulation)
		/// This will allow us to avoid a bunch of boundary check conditions inside the heavy duty update code
		/// </summary>
		Cell m_cellGrid[PhysicsConfiguration::SpaceGridCellMoleculesWidth + 2][PhysicsConfiguration::SpaceGridCellMoleculesHeight + 2];
	};
};

