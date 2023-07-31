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

void SpaceGridMolecules::ResetMoleculeCellLocations(unsigned columnStartID, unsigned columnLastID)
{
	// remember to offset any cell access by index by one because of the empty starting cells
	if (columnLastID == PhysicsConfiguration::SpaceGridCellMoleculesWidth - 1)
	{
		// Add one to also erase the "hidden" last column
		columnLastID += 1;
	}
	// Always add one to offset the "hidden" first column at index 0
	columnLastID += 1;
	
	// Keep start index to 0 to clear also the "hidden" starting column if this work will clear the first real column
	if (columnStartID != 0)
	{
		columnStartID += 1;
	}

	for (unsigned widthIndex = columnStartID; widthIndex <= columnLastID; widthIndex++)
	{
		for (unsigned heightIndex = 0; heightIndex < PhysicsConfiguration::SpaceGridCellMoleculesHeight + 2; heightIndex++)
		{
			m_cellGrid[widthIndex][heightIndex].MoleculeIDs.clear();
		}
	}
}

void SpaceGridMolecules::UpdateMoleculesCellLocation(std::vector<GasMolecules::GasMolecule*>& molecules, unsigned startIndex, unsigned lastIndex)
{
	// TODO: K.L. Implement worker thread startIndex, lastIndex
	for (unsigned moleculeID = startIndex; moleculeID <= lastIndex; moleculeID++)
	{
		GasMolecules::GasMolecule* molecule = molecules[moleculeID];
		// remember to offset any cell access by index by one because of the empty starting cells
		molecule->cellIndexX = ((molecule->position.x + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.x) / CellWidth);
		molecule->cellIndexY = ((molecule->position.y + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.y) / CellHeight);
		m_cellGrid[molecule->cellIndexX + 1][molecule->cellIndexY + 1].MoleculeIDs.push_back(moleculeID);
	}
}

SpaceGridMolecules::Cell* SpaceGridMolecules::GetCell(unsigned indexX, unsigned indexY)
{
	// remember to offset any cell access by index by one because of the empty starting cells
	return &m_cellGrid[indexX + 1][indexY + 1];
}