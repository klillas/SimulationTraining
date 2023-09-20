#version 430

#define INVOCATION_SIZE_X 32
// TODO: Connect all configuration to their respective source values

// PhysicsConfiguration::PhysicsConfiguration_GasMoleculeMax
#define PhysicsConfiguration_GasMoleculeMax 64
// PhysicsConfiguration::PhysicsConfiguration_SpaceGridCellMoleculesWidth
#define PhysicsConfiguration_SpaceGridCellMoleculesWidth 4
// PhysicsConfiguration::PhysicsConfiguration_SpaceGridCellMoleculesHeight
#define PhysicsConfiguration_SpaceGridCellMoleculesHeight 4

layout(local_size_x = INVOCATION_SIZE_X, local_size_y = 1) in;

// No push constant is used right now, we store what we need in the shader buffer struct
layout (push_constant) uniform ComputeShaderPushConstants {
    uint InBufferSize_items;
} push_constants;


struct Molecule {
    float positionX;
	float positionY;

    float velocityX;
	float velocityY;

	uint gridCellPositionX;
	uint gridCellPositionY;
};

struct ParticleGrid {
	uint cellCountX;
	uint cellCountY;
	float cellWidth;
	float cellHeight;

	uint moleculeCellMapping[PhysicsConfiguration_GasMoleculeMax];
	uint cellStartIndex[PhysicsConfiguration_SpaceGridCellMoleculesWidth][PhysicsConfiguration_SpaceGridCellMoleculesHeight];
	uint cellMoleculeCount[PhysicsConfiguration_SpaceGridCellMoleculesWidth][PhysicsConfiguration_SpaceGridCellMoleculesHeight];
};

struct PhysicsState {
	float physicsEngineStartPosNegativeDeltaX;
	float physicsEngineStartPosNegativeDeltaY;
	uint moleculeCount;
	float moleculeRadius;
	float physicsTickTimeDelta;
	Molecule molecules[PhysicsConfiguration_GasMoleculeMax];
	ParticleGrid particleGrid;
};

struct WorldState {
	PhysicsState physicsState;

	uint debugOut1;
	uint debugOut2;
	uint debugOut3;
	uint debugOut4;
	uint debugOut5;
};

layout(std430, binding = 0) buffer lay0 { WorldState worldStateIn; };
layout(std430, binding = 1) buffer lay1 { WorldState worldStateOut; };

void UpdateMoleculePosition(uint moleculeIndex);
void ResolveMoleculeWallInteraction(uint moleculeIndex);
void ResolveMoleculeCollisions(uint moleculeIndex);
void UpdateMoleculeCellLookupTable();

void main() {
	const uint globalInvocationID = gl_GlobalInvocationID.x;
	const uint totalGlobalInvocations = gl_NumWorkGroups.x * INVOCATION_SIZE_X;
	const uint itemsInBuf = worldStateIn.physicsState.moleculeCount;
	// Assume exact division to avoid if case later. CPU side needs to configure amount of items correctly
	const uint itemsPerThread = itemsInBuf / totalGlobalInvocations;

	//worldStateIn.physicsState.MoleculeCount = worldStateIn.physicsState.MoleculeCount * 2;
	//worldStateIn.physicsState.MoleculeCount = 123;

/*
	if (globalInvocationID == 0)
	{
		worldStateIn.debugOut1++;
	}
*/
	/*
	if (globalInvocationID == 0)
	{
		worldStateIn.debugOut1 = worldStateIn.physicsState.particleGrid.CellCountX;
		worldStateIn.debugOut2 = worldStateIn.physicsState.particleGrid.CellCountY;
		worldStateIn.debugOut3 = 12345;
		worldStateIn.debugOut4 = 4321;
		worldStateIn.debugOut5 = 13579;
	}
	*/

	for (uint i = (globalInvocationID * itemsPerThread); i < ((globalInvocationID * itemsPerThread) + itemsPerThread); i++)
	{
		// TODO: Remove multiply of physicsticktimedelta if we know the delta velocity

		// Update position according to velocity of molecule
		UpdateMoleculePosition(i);

		// Molecule collision resolution
		ResolveMoleculeCollisions(i);
		
		// Check wall boundaries and bounce if intersect
		ResolveMoleculeWallInteraction(i);
	}

	// Synchronize all shader invocations before continuing
	memoryBarrierShared();
	barrier();

	// Single threaded update of molecule lookup table
	if (globalInvocationID == 0)
	{
		UpdateMoleculeCellLookupTable();
	}

	memoryBarrierShared();
	barrier();	

	//worldStateIn.physicsState.Molecules[0].PositionX += worldStateIn.physicsState.Molecules[0].VelocityX;
}


void UpdateMoleculePosition(uint moleculeIndex)
{
	worldStateIn.physicsState.molecules[moleculeIndex].positionX += (worldStateIn.physicsState.molecules[moleculeIndex].velocityX * worldStateIn.physicsState.physicsTickTimeDelta);
	worldStateIn.physicsState.molecules[moleculeIndex].positionY += (worldStateIn.physicsState.molecules[moleculeIndex].velocityY * worldStateIn.physicsState.physicsTickTimeDelta);
}

void ResolveMoleculeWallInteraction(uint moleculeIndex)
{
		if (worldStateIn.physicsState.molecules[moleculeIndex].positionX > 1)
		{
			worldStateIn.physicsState.molecules[moleculeIndex].positionX = (1.0f - worldStateIn.physicsState.moleculeRadius);
			worldStateIn.physicsState.molecules[moleculeIndex].velocityX = -worldStateIn.physicsState.molecules[moleculeIndex].velocityX;
		}
		else if (worldStateIn.physicsState.molecules[moleculeIndex].positionX < -1)
		{
			worldStateIn.physicsState.molecules[moleculeIndex].positionX = (-1.0f + worldStateIn.physicsState.moleculeRadius);
			worldStateIn.physicsState.molecules[moleculeIndex].velocityX = -worldStateIn.physicsState.molecules[moleculeIndex].velocityX;
		}

		if (worldStateIn.physicsState.molecules[moleculeIndex].positionY > 1)
		{
			worldStateIn.physicsState.molecules[moleculeIndex].positionY = (1.0f - worldStateIn.physicsState.moleculeRadius);
			worldStateIn.physicsState.molecules[moleculeIndex].velocityY = -worldStateIn.physicsState.molecules[moleculeIndex].velocityY;
		}
		else if (worldStateIn.physicsState.molecules[moleculeIndex].positionY < -1)
		{
			worldStateIn.physicsState.molecules[moleculeIndex].positionY = (-1.0f + worldStateIn.physicsState.moleculeRadius);
			worldStateIn.physicsState.molecules[moleculeIndex].velocityY = -worldStateIn.physicsState.molecules[moleculeIndex].velocityY;
		}	
}

void ResolveMoleculeCollisions(uint moleculeIndex)
{
}

void UpdateMoleculeCellLookupTable()
{
	// TODO: At least some of these loops could be parallelized, but it is uncertain if it is worth the hassle.
	//       This is an O(n) function, we only traverse the molecules and cell lookup grid a few times

	// Reset lookup addresses for the particle grid, we will incrementally increase the size requirement for each cell in the grid
	for (uint width = 0; width < worldStateIn.physicsState.particleGrid.cellCountX; width++)
	{
		for (uint height = 0; height < worldStateIn.physicsState.particleGrid.cellCountY; height++)
		{
			worldStateIn.physicsState.particleGrid.cellStartIndex[width][height] = 0;
			worldStateIn.physicsState.particleGrid.cellMoleculeCount[width][height] = 0;
		}
	}

	for (uint moleculeID = 0; moleculeID < worldStateIn.physicsState.moleculeCount; moleculeID++)
	{
		// Store the cell position index for each molecule
		worldStateIn.physicsState.molecules[moleculeID].gridCellPositionX = 
			uint((worldStateIn.physicsState.molecules[moleculeID].positionX + worldStateIn.physicsState.physicsEngineStartPosNegativeDeltaX) / worldStateIn.physicsState.particleGrid.cellWidth);
		worldStateIn.physicsState.molecules[moleculeID].gridCellPositionY = 
			uint((worldStateIn.physicsState.molecules[moleculeID].positionY + worldStateIn.physicsState.physicsEngineStartPosNegativeDeltaY) / worldStateIn.physicsState.particleGrid.cellHeight);

		// Make sure the lookup table will assign enough memory for each cell -- molecule lookup table
		// Because of the one dimensional preallocated lookup memory we need to know exactly where each cell lookup begins and where it ends when we later store the molecule indexes in there
		worldStateIn.physicsState.particleGrid.cellMoleculeCount[worldStateIn.physicsState.molecules[moleculeID].gridCellPositionX][worldStateIn.physicsState.molecules[moleculeID].gridCellPositionY]++;
	}

	// We know how much space each cell needs in the lookup table, calculate the start addresses for each cell and reset the cellMoleculeCount
	// The cellMoleculeCount is used to hold the next free lookup location when we assign the molecule indexes to each cell
	uint nextFreeAddress = 0;
	for (uint width = 0; width < worldStateIn.physicsState.particleGrid.cellCountX; width++)
	{
		for (uint height = 0; height < worldStateIn.physicsState.particleGrid.cellCountY; height++)
		{
			worldStateIn.physicsState.particleGrid.cellStartIndex[width][height] = nextFreeAddress;
			nextFreeAddress += worldStateIn.physicsState.particleGrid.cellMoleculeCount[width][height];
			worldStateIn.physicsState.particleGrid.cellMoleculeCount[width][height] = 0;
		}
	}

	// The cell-molecule lookup table start addresses are known, and we also know the amount of molecules in every cell
	// Traverse all molecules and add their IDs to the correct cell-molecule lookup table locations
	// Also increment the cellMoleculeCount each time a molecule is added to the cell to keep track of the next free lookup location in that cell
	uint moleculeCellMappingLocation = 0;
	uint cellIndexX = 0;
	uint cellIndexY = 0;
	for (uint moleculeID = 0; moleculeID < worldStateIn.physicsState.moleculeCount; moleculeID++)
	{
		cellIndexX = worldStateIn.physicsState.molecules[moleculeID].gridCellPositionX;
		cellIndexY = worldStateIn.physicsState.molecules[moleculeID].gridCellPositionY;
		moleculeCellMappingLocation = 
			worldStateIn.physicsState.particleGrid.cellStartIndex[cellIndexX][cellIndexY] + 
			worldStateIn.physicsState.particleGrid.cellMoleculeCount[cellIndexX][cellIndexY];
		worldStateIn.physicsState.particleGrid.cellMoleculeCount[cellIndexX][cellIndexY]++;
		
		worldStateIn.physicsState.particleGrid.moleculeCellMapping[moleculeCellMappingLocation] = moleculeID;
	}

	// worldStateIn.physicsState.particleGrid now contains a complete 2d grid with cells and which molecule exist under each cell
}

