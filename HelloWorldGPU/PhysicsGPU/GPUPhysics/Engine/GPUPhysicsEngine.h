#pragma once
#include <cstdint>

#include "Physics/PhysicsConfiguration.h"
#include <vector>
#include "vulkan/VulkanInit.h"

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
                };

                uint32_t MoleculeCount;
                float MoleculeRadius;
                float PhysicsTickTimeDelta;
                Molecule Molecules[Physics::PhysicsConfiguration::GasMoleculeMax];
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

        void AddMolecule(float positionX, float positionY, float velocityX, float velocityY, float colorR, float colorG, float colorB);

        unsigned GasMoleculeCount();

        void GetVertices(std::vector<VulkanInit::Vertex>* verticesBuffer);

    private:
        void GetMoleculeVertices(WorldState::PhysicsState::Molecule* molecule, MoleculeAppearance* moleculeAppearance, std::vector<VulkanInit::Vertex>* vertexVector);

        WorldState m_worldState;
        MoleculeAppearance m_moleculeAppearances[Physics::PhysicsConfiguration::GasMoleculeMax];
	};
};
