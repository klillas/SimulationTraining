#pragma once
#include <cstdint>

#include "Physics/PhysicsConfiguration.h"
#include <vector>
#include "vulkan/VulkanInit.h"

using namespace Physics;

namespace PhysicsGPU::Engine
{
	class GPUPhysicsEngine
	{
    public:
        /// <summary>
        /// Contains all necessary information of the physics world state at this moment in time
        /// If not external stimuli happens (like user interaction), this is all information the physics engine needs to step forward in the simulation
        /// </summary>
        struct WorldState
        {
            struct PhysicsState {
                struct Molecule
                {
                    float PositionX;
                    float PositionY;

                    float VelocityX;
                    float VelocityY;

                    uint32_t gridCellPositionX;
                    uint32_t gridCellPositionY;
                };

                const float physicsEngineStartPosNegativeDeltaX = PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.x;
                const float physicsEngineStartPosNegativeDeltaY = PhysicsConfiguration::PhysicsEngineStartPosNegativeDelta.y;
                uint32_t MoleculeCount;
                float MoleculeRadius;
                float PhysicsTickTimeDelta;
                Molecule Molecules[Physics::PhysicsConfiguration::GasMoleculeMax];

                struct ParticleGrid
                {
                    const unsigned CellCountX = PhysicsConfiguration::SpaceGridCellMoleculesWidth;
                    const unsigned CellCountY = PhysicsConfiguration::SpaceGridCellMoleculesHeight;
                    const float CellWidth = PhysicsConfiguration::PhysicsEngineWidth / PhysicsConfiguration::SpaceGridCellMoleculesWidth;
                    const float CellHeight = PhysicsConfiguration::PhysicsEngineHeight / PhysicsConfiguration::SpaceGridCellMoleculesHeight;

                    /// <summary>
                    /// Keep track of what molecule goes to what cell in the ParticleGrid here.
                    /// The start and end index of each cell is maintained separately
                    /// This approach allows a dynamic amount of molecules in each cell, theoretically all molecules could be in a single cell
                    /// </summary>
                    uint32_t MoleculeCellMapping[Physics::PhysicsConfiguration::GasMoleculeMax];

                    /// <summary>
                    /// The MoleculeCellMapping start index is stored here for each cell
                    /// CellStartIndex = CellStartIndex = 0 if the cell does not contain any molecule
                    /// </summary>
                    uint32_t CellStartIndex[PhysicsConfiguration::SpaceGridCellMoleculesWidth][PhysicsConfiguration::SpaceGridCellMoleculesHeight];

                    /// <summary>
                    /// Amount of molecules in each cell
                    /// </summary>
                    uint32_t CellMoleculeCount[PhysicsConfiguration::SpaceGridCellMoleculesWidth][PhysicsConfiguration::SpaceGridCellMoleculesHeight];
                };
                ParticleGrid particleGrid;
            };
            PhysicsState physicsState;

            uint32_t debugOut1 = 0;
            uint32_t debugOut2 = 0;
            uint32_t debugOut3 = 0;
            uint32_t debugOut4 = 0;
            uint32_t debugOut5 = 0;
        };

        struct MoleculeAppearance
        {
            float ColorR;
            float ColorG;
            float ColorB;
        };

        GPUPhysicsEngine();

        void PhysicsTick(float timeDelta, uint32_t subSteps);

        void VulkanGetComputeBuffersCallback(uint8_t*& buffer, uint32_t& bufferSizeBytes);

        void VulkanPostComputeBufferTickCallback(void* gpuBuffer);

        void VulkanPreComputeBufferTickCallback(void* gpuBuffer);

        void AddMolecule(float positionX, float positionY, float velocityX, float velocityY, float colorR, float colorG, float colorB);

        unsigned GasMoleculeCount();

        void GetVertices(std::vector<VulkanInit::Vertex>* verticesBuffer);

    private:
        void GetMoleculeVertices(WorldState::PhysicsState::Molecule* molecule, MoleculeAppearance* moleculeAppearance, std::vector<VulkanInit::Vertex>* vertexVector);

        //void Copy

        WorldState m_worldState;
        MoleculeAppearance m_moleculeAppearances[Physics::PhysicsConfiguration::GasMoleculeMax];
	};
};
