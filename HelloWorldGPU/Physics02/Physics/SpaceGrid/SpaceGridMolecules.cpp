#include "SpaceGridMolecules.h"

using namespace Physics::SpaceGrid;

SpaceGridMolecules::SpaceGridMolecules()
{
	// Setup an empty start / end molecule for every cell
	// This avoids a bunch of null checks later in the code
	for (unsigned i = 0; i < PhysicsConfiguration::SpaceGridMoleculesWidth + 2; i++)
	{
		for (unsigned j = 0; j < PhysicsConfiguration::SpaceGridMoleculesHeight + 2; j++)
		{
			m_startEndMolecules[i][j][0].nextItem = &m_startEndMolecules[i][j][1];
			m_startEndMolecules[i][j][1].previousItem = &m_startEndMolecules[i][j][0];

			m_cellGrid[i][j].StartMolecule = &m_startEndMolecules[i][j][0];
			m_cellGrid[i][j].StopMolecule = &m_startEndMolecules[i][j][1];
		}
	}
}

void SpaceGridMolecules::AddGasMolecule(GasMolecules::GasMolecule* newMolecule)
{
	// remember to offset any cell access by index by one because of the empty starting cells
	unsigned xCellIndex = ((newMolecule->position.x + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.x) / CellWidth);
	unsigned yCellIndex = ((newMolecule->position.y + PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.y) / CellHeight);

	// Add molecule to the correct starting cell
	m_cellGrid[xCellIndex + 1][yCellIndex + 1].MoleculeCount++;
	GasMolecules::GasMolecule* startMolecule = m_cellGrid[xCellIndex + 1][yCellIndex + 1].StartMolecule;
	GasMolecules::GasMolecule* nextMolecule = startMolecule->nextItem;

	newMolecule->cellIndexX = xCellIndex;
	newMolecule->cellIndexY = yCellIndex;

	startMolecule->nextItem = newMolecule;
	newMolecule->previousItem = startMolecule;
	newMolecule->nextItem = nextMolecule;
	nextMolecule->previousItem = newMolecule;
}

void SpaceGridMolecules::UpdateMoleculeCellLocation(GasMolecules::GasMolecule* molecule)
{
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
}

SpaceGridMolecules::Cell* SpaceGridMolecules::GetCell(unsigned indexX, unsigned indexY)
{
	// remember to offset any cell access by index by one because of the empty starting cells
	return &m_cellGrid[indexX + 1][indexY + 1];
}