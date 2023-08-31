#version 430

#define INVOCATION_SIZE_X 32
// TODO: Connect gas molecule max with PhysicsConfiguration::PhysicsConfiguration_GasMoleculeMax
#define PhysicsConfiguration_GasMoleculeMax 8192

layout(local_size_x = INVOCATION_SIZE_X, local_size_y = 1) in;

// No push constant is used right now, we store what we need in the shader buffer struct
layout (push_constant) uniform ComputeShaderPushConstants {
    uint InBufferSize_items;
} push_constants;


struct Molecule {
    float PositionX;
	float PositionY;

    float VelocityX;
	float VelocityY;
};

struct PhysicsState {
	uint MoleculeCount;
	float MoleculeRadius;
	float PhysicsTickTimeDelta;
	Molecule Molecules[PhysicsConfiguration_GasMoleculeMax];
};

struct WorldState {
	PhysicsState PhysicsState;

	uint debugOut1;
	uint debugOut2;
	uint debugOut3;
	uint debugOut4;
	uint debugOut5;
};

layout(std430, binding = 0) buffer lay0 { WorldState worldStateIn; };
layout(std430, binding = 1) buffer lay1 { WorldState worldStateOut; };

void main() {
	const uint globalInvocationID = gl_GlobalInvocationID.x;
	const uint totalGlobalInvocations = gl_NumWorkGroups.x * INVOCATION_SIZE_X;
	const uint itemsInBuf = worldStateIn.PhysicsState.MoleculeCount;
	// Assume exact division to avoid if case later. CPU side needs to configure amount of items correctly
	const uint itemsPerThread = itemsInBuf / totalGlobalInvocations;

	//worldStateIn.PhysicsState.MoleculeCount = worldStateIn.PhysicsState.MoleculeCount * 2;
	//worldStateIn.PhysicsState.MoleculeCount = 123;
	
	/*
	if (globalInvocationID == 0)
	{
		worldStateIn.debugOut1 = globalInvocationID;
		worldStateIn.debugOut2 = itemsPerThread;
		worldStateIn.debugOut3 = itemsInBuf;
		worldStateIn.debugOut4 = gl_NumWorkGroups.x;
		worldStateIn.debugOut5 = INVOCATION_SIZE_X;
	}
	*/
	

	for (uint i = (globalInvocationID * itemsPerThread); i < ((globalInvocationID * itemsPerThread) + itemsPerThread); i++)
	{
		// TODO: Remove multiply of physicsticktimedelta if we know the delta velocity

		// Update position according to velocity of molecule
		worldStateIn.PhysicsState.Molecules[i].PositionX += (worldStateIn.PhysicsState.Molecules[i].VelocityX * worldStateIn.PhysicsState.PhysicsTickTimeDelta);
		worldStateIn.PhysicsState.Molecules[i].PositionY += (worldStateIn.PhysicsState.Molecules[i].VelocityY * worldStateIn.PhysicsState.PhysicsTickTimeDelta);
		
		// Check wall boundaries and bounce if intersect
		if (worldStateIn.PhysicsState.Molecules[i].PositionX > 1)
		{
			worldStateIn.PhysicsState.Molecules[i].PositionX = (1.0f - worldStateIn.PhysicsState.MoleculeRadius);
			worldStateIn.PhysicsState.Molecules[i].VelocityX = -worldStateIn.PhysicsState.Molecules[i].VelocityX;
		}
		else if (worldStateIn.PhysicsState.Molecules[i].PositionX < -1)
		{
			worldStateIn.PhysicsState.Molecules[i].PositionX = (-1.0f + worldStateIn.PhysicsState.MoleculeRadius);
			worldStateIn.PhysicsState.Molecules[i].VelocityX = -worldStateIn.PhysicsState.Molecules[i].VelocityX;
		}

		if (worldStateIn.PhysicsState.Molecules[i].PositionY > 1)
		{
			worldStateIn.PhysicsState.Molecules[i].PositionY = (1.0f - worldStateIn.PhysicsState.MoleculeRadius);
			worldStateIn.PhysicsState.Molecules[i].VelocityY = -worldStateIn.PhysicsState.Molecules[i].VelocityY;
		}
		else if (worldStateIn.PhysicsState.Molecules[i].PositionY < -1)
		{
			worldStateIn.PhysicsState.Molecules[i].PositionY = (-1.0f + worldStateIn.PhysicsState.MoleculeRadius);
			worldStateIn.PhysicsState.Molecules[i].VelocityY = -worldStateIn.PhysicsState.Molecules[i].VelocityY;
		}
	}

	//worldStateIn.PhysicsState.Molecules[0].PositionX += worldStateIn.PhysicsState.Molecules[0].VelocityX;
}

