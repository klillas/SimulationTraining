#include "SpaceGridMolecules.h"

using namespace Physics::SpaceGrid;

SpaceGridMolecules::SpaceGridMolecules()
{
	assert(CellWidth > PhysicsConfiguration::GasMoleculeDiameter);
	assert(CellHeight > PhysicsConfiguration::GasMoleculeDiameter);
}

void SpaceGridMolecules::RemoveGasMolecule(GasMolecules::GasMolecule* molecule, unsigned moleculeID)
{
	// TODO: K.L. Add support for concurrent_vector
	assert(false);
	/*
	// TODO: This can probably be expensive if the vector is long? Test it and see that there is no major overhead
	Cell* cell = &m_cellGrid[molecule->cellIndexX + 1][molecule->cellIndexY + 1];

	// Remove the molecule from the cell
	for (tbb::concurrent_vector<unsigned>::iterator it = cell->MoleculeIDs.begin(); it != cell->MoleculeIDs.end();)
	{
		if (*it == moleculeID)
		{
			it = cell->MoleculeIDs.erase(it);
			break;
		}
		else
		{
			++it;
		}
	}

	// Reset any cell specific information in molecule
	molecule->cellIndexX = 0;
	molecule->cellIndexY = 0;
	*/
}

void SpaceGridMolecules::AddGasMolecule(GasMolecules::GasMolecule* newMolecule, unsigned moleculeID)
{
	// remember to offset any cell access by index by one because of the empty starting cells
	unsigned xCellIndex = ((newMolecule->position.x + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.x) / CellWidth);
	unsigned yCellIndex = ((newMolecule->position.y + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.y) / CellHeight);

	// Add molecule to the correct starting cell
	m_cellGrid[xCellIndex + 1][yCellIndex + 1].MoleculeIDs.push_back(moleculeID);

	newMolecule->cellIndexX = xCellIndex;
	newMolecule->cellIndexY = yCellIndex;
}

void SpaceGridMolecules::UpdateMoleculesCellLocation(std::vector<GasMolecules::GasMolecule*>& molecules)
{
	// First reset all cells
	for (unsigned widthIndex = 0; widthIndex < PhysicsConfiguration::SpaceGridMoleculesWidth + 2; widthIndex++)
	{
		for (unsigned heightIndex = 0; heightIndex < PhysicsConfiguration::SpaceGridMoleculesHeight + 2; heightIndex++)
		{
			m_cellGrid[widthIndex][heightIndex].MoleculeIDs.clear();
		}
	}

	for (unsigned moleculeID = 0; moleculeID < molecules.size(); moleculeID++)
	{
		GasMolecules::GasMolecule* molecule = molecules[moleculeID];
		// remember to offset any cell access by index by one because of the empty starting cells
		molecule->cellIndexX = ((molecule->position.x + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.x) / CellWidth);
		molecule->cellIndexY = ((molecule->position.y + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.y) / CellHeight);
		m_cellGrid[molecule->cellIndexX + 1][molecule->cellIndexY + 1].MoleculeIDs.push_back(moleculeID);
	}


	/*
	// remember to offset any cell access by index by one because of the empty starting cells
	unsigned xCellIndex = ((molecule->position.x + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.x) / CellWidth);
	unsigned yCellIndex = ((molecule->position.y + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.y) / CellHeight);

	// Remove molecule from the current cell
	GasMolecules::GasMolecule* prevMolecule = molecule->previousItem;
	GasMolecules::GasMolecule* nextMolecule = molecule->nextItem;
	prevMolecule->nextItem = nextMolecule;
	nextMolecule->previousItem = prevMolecule;
	// Reduce molecule count in current cell by one
	m_cellGrid[molecule->cellIndexX + 1][molecule->cellIndexY + 1].MoleculeCount--;

	// Add molecule to the new cell (it can be the same cell, cheaper to reassign than to run if checks)
	GasMolecules::GasMolecule* startMolecule = m_cellGrid[xCellIndex + 1][yCellIndex + 1].StartMolecule;
	nextMolecule = startMolecule->nextItem;

	startMolecule->nextItem = molecule;
	molecule->previousItem = startMolecule;
	molecule->nextItem = nextMolecule;
	nextMolecule->previousItem = molecule;
	//Update cell index in molecule and add one to the cell count
	molecule->cellIndexX = xCellIndex;
	molecule->cellIndexY = yCellIndex;
	m_cellGrid[xCellIndex + 1][yCellIndex + 1].MoleculeCount++;
	*/
}

SpaceGridMolecules::Cell* SpaceGridMolecules::GetCell(unsigned indexX, unsigned indexY)
{
	// remember to offset any cell access by index by one because of the empty starting cells
	return &m_cellGrid[indexX + 1][indexY + 1];
}