#include "GPUPhysicsEngine.h"
#include <vector>
#include <Vulkan/VulkanInit.h>
#include "Physics/PhysicsConfiguration.h"

using namespace PhysicsGPU::Engine;
using namespace Physics;

GPUPhysicsEngine::GPUPhysicsEngine()
{
	m_worldState.physicsState.MoleculeCount = 0;
    m_worldState.physicsState.MoleculeRadius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;
    m_worldState.physicsState.PhysicsTickTimeDelta = 1.0f / 60.0f;
}

void GPUPhysicsEngine::AddMolecule(float positionX, float positionY, float velocityX, float velocityY, float colorR, float colorG, float colorB)
{
	assert(m_worldState.physicsState.MoleculeCount < Physics::PhysicsConfiguration::GasMoleculeMax);
	// TODO: Add limit checks on input params

	uint32_t index = m_worldState.physicsState.MoleculeCount;
	WorldState::PhysicsState::Molecule* molecule = &m_worldState.physicsState.Molecules[index];
    MoleculeAppearance* moleculeAppearance = &m_moleculeAppearances[index];
	molecule->PositionX = positionX;
	molecule->PositionY = positionY;
	molecule->VelocityX = velocityX;
	molecule->VelocityY = velocityY;
    moleculeAppearance->ColorR = colorR;
    moleculeAppearance->ColorG = colorG;
    moleculeAppearance->ColorB = colorB;

	m_worldState.physicsState.MoleculeCount++;
}

void GPUPhysicsEngine::PhysicsTick(float timeDelta, uint32_t subSteps)
{
	// TODO: Implement
}

void GPUPhysicsEngine::VulkanGetComputeBuffersCallback(uint8_t*& buffer, uint32_t& bufferSizeBytes)
{
	buffer = (uint8_t*)&m_worldState;
	bufferSizeBytes = sizeof(m_worldState);
}

void GPUPhysicsEngine::VulkanPostComputeBufferTickCallback(void* gpuBuffer)
{
    // Only copy the molecules we want to render back to CPU
    unsigned maxMolecules = m_worldState.physicsState.MoleculeCount / PhysicsConfiguration::PhysicsEngineFilterMoleculeFactor;
    uintptr_t offset = reinterpret_cast<uintptr_t>(&m_worldState.physicsState.Molecules) - reinterpret_cast<uintptr_t>(&m_worldState);
    unsigned bytesToTransfer = maxMolecules * sizeof(WorldState::PhysicsState::Molecule);
    memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&m_worldState) + offset),
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(gpuBuffer) + offset),
        bytesToTransfer);

    // Debug purposes, copy cell - molecule mapping back to CPU
    offset = reinterpret_cast<uintptr_t>(&m_worldState.physicsState.particleGrid) - reinterpret_cast<uintptr_t>(&m_worldState);
    bytesToTransfer = sizeof(WorldState::PhysicsState::ParticleGrid);
    memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&m_worldState) + offset),
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(gpuBuffer) + offset),
        bytesToTransfer);

    // TODO: Not safe to assume the debug outputs are next to each other. Create an array of debug outputs instead
    // Copy debug output
    offset = reinterpret_cast<uintptr_t>(&m_worldState.debugOut1) - reinterpret_cast<uintptr_t>(&m_worldState);
    bytesToTransfer = 5 * sizeof(WorldState::debugOut1);
    memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(&m_worldState) + offset),
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(gpuBuffer) + offset),
        bytesToTransfer);
}

void GPUPhysicsEngine::VulkanPreComputeBufferTickCallback(void* gpuBuffer)
{
    // TODO: Send over only the new state changes that happened on the CPU side
    //memcpy(gpuBuffer, (void*)&m_worldState, sizeof(m_worldState));
}

unsigned GPUPhysicsEngine::GasMoleculeCount()
{
	return m_worldState.physicsState.MoleculeCount;
}

void GPUPhysicsEngine::GetVertices(std::vector<VulkanInit::Vertex>* verticesBuffer)
{
	verticesBuffer->clear();

	//for (unsigned i = 0; i < m_worldState.PhysicsState.MoleculeCount; i = i + PhysicsConfiguration::PhysicsEngineFilterMoleculeFactor)
    unsigned maxMolecules = m_worldState.physicsState.MoleculeCount / PhysicsConfiguration::PhysicsEngineFilterMoleculeFactor;

    for (unsigned i = 0; i < maxMolecules; i++)
	{
        GetMoleculeVertices(&m_worldState.physicsState.Molecules[i], &m_moleculeAppearances[i], verticesBuffer);
	}
}

void GPUPhysicsEngine::GetMoleculeVertices(WorldState::PhysicsState::Molecule* molecule, MoleculeAppearance* moleculeAppearance, std::vector<VulkanInit::Vertex>* vertexVector)
{
    glm::vec2 center(molecule->PositionX, molecule->PositionY);
    static const float radius = PhysicsConfiguration::GasMoleculeDiameter / 2.0f;

    // Define the corner positions relative to the center
    glm::vec2 topLeft(center.x - radius, center.y - radius);
    glm::vec2 topRight(center.x + radius, center.y - radius);
    glm::vec2 bottomRight(center.x + radius, center.y + radius);
    glm::vec2 bottomLeft(center.x - radius, center.y + radius);

    VulkanInit::Vertex vertex;
    vertex.pos = topLeft;
    vertex.color.r = moleculeAppearance->ColorR;
    vertex.color.g = moleculeAppearance->ColorG;
    vertex.color.b = moleculeAppearance->ColorB;
    vertexVector->push_back(vertex);

    VulkanInit::Vertex vertex2;
    vertex2.pos = topRight;
    vertex2.color.r = moleculeAppearance->ColorR;
    vertex2.color.g = moleculeAppearance->ColorG;
    vertex2.color.b = moleculeAppearance->ColorB;
    vertexVector->push_back(vertex2);

    VulkanInit::Vertex vertex3;
    vertex3.pos = bottomRight;
    vertex3.color.r = moleculeAppearance->ColorR;
    vertex3.color.g = moleculeAppearance->ColorG;
    vertex3.color.b = moleculeAppearance->ColorB;
    vertexVector->push_back(vertex3);

    VulkanInit::Vertex vertex4;
    vertex4.pos = bottomLeft;
    vertex4.color.r = moleculeAppearance->ColorR;
    vertex4.color.g = moleculeAppearance->ColorG;
    vertex4.color.b = moleculeAppearance->ColorB;
    vertexVector->push_back(vertex4);

    VulkanInit::Vertex vertex5;
    vertex5.pos = topLeft;
    vertex5.color.r = moleculeAppearance->ColorR;
    vertex5.color.g = moleculeAppearance->ColorG;
    vertex5.color.b = moleculeAppearance->ColorB;
    vertexVector->push_back(vertex5);

    VulkanInit::Vertex vertex6;
    vertex6.pos = bottomRight;
    vertex6.color.r = moleculeAppearance->ColorR;
    vertex6.color.g = moleculeAppearance->ColorG;
    vertex6.color.b = moleculeAppearance->ColorB;
    vertexVector->push_back(vertex6);
}