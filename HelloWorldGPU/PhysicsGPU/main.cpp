/*
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
*/

#define RUN_TEST_ID 1

#include <sstream>
#include <iomanip>
#include "Vulkan/VulkanInit.h"
#include "./Debug/DebugOutput.h"
#include "./Physics/Engine/IPhysicsEngine.h"
#include "./Physics/Engine/PhysicsEngine.h"
#include "./Physics/PhysicsConfiguration.h"
#include "GPUPhysics/Engine/GPUPhysicsEngine.h"
#include <random>
#include <glm/glm.hpp>

using namespace Physics::RigidObjects;

Physics::Engine::IPhysicsEngine* engine = new Physics::Engine::PhysicsEngine();
PhysicsGPU::Engine::GPUPhysicsEngine* physicsGPUEngine = new PhysicsGPU::Engine::GPUPhysicsEngine();

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> distributionPosition(-1.0f, 1.0f);
std::uniform_real_distribution<float> distributionColor(0.0f, 1.0f);
std::uniform_real_distribution<float> distributionVelocity(-5.0f, 5.0f);


void VulkanFrameRenderedCallback(unsigned frametime_ms)
{
    static unsigned frametime_total = 0;
    static unsigned framecounter = 0;
    static Debug::DebugOutput* debug = Debug::DebugOutput::GetInstance();

    frametime_total += frametime_ms;
    framecounter++;

    if (frametime_total >= 1000)
    {
        float framesPerSecond = framecounter * 1000.0f / frametime_total;

        std::string str = "Frames per second: ";
        // Convert float to string
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << framesPerSecond;
        std::string floatString = ss.str();
        // Concatenate the string and float string
        std::string result = str + floatString;
        debug->Print(&result);

#if RUN_TEST_ID == 1
        if (framesPerSecond > 55)
        {
            unsigned newMoleculeSize = physicsGPUEngine->GasMoleculeCount() * 0.05f + 1;
            // Must be evenly divisible with 64
            newMoleculeSize = newMoleculeSize + 64 - (newMoleculeSize % 64);
            for (unsigned i = 0; i < newMoleculeSize; i++)
            {
                physicsGPUEngine->AddMolecule(
                    distributionPosition(gen), 
                    distributionPosition(gen), 
                    distributionVelocity(gen), 
                    distributionVelocity(gen),
                    distributionColor(gen),
                    distributionColor(gen),
                    distributionColor(gen)
                );
            }
        }
#endif

        std::string str2 = "Molecules: ";
        // Convert float to string
        std::stringstream ss2;
        ss2 << std::fixed << std::setprecision(1) << physicsGPUEngine->GasMoleculeCount();
        std::string floatString2 = ss2.str();
        // Concatenate the string and float string
        std::string result2 = str2 + floatString2;
        debug->Print(&result2);

        framecounter = 0;
        frametime_total = 0;
    }
}

void VulkanFrameRenderStartCallback()
{
    physicsGPUEngine->PhysicsTick(0.005f, 1);
}

void VulkanGetVerticesCallback(std::vector<VulkanInit::Vertex>* verticesBuffer)
{
    verticesBuffer->clear();
    physicsGPUEngine->GetVertices(verticesBuffer);
}

void VulkanGetComputeBuffersCallback(uint8_t*& buffer, uint32_t& bufferSizeBytes)
{
    physicsGPUEngine->VulkanGetComputeBuffersCallback(buffer, bufferSizeBytes);
}


int main() {
    VulkanInit app;
    app.RegisterVulkanFrameRenderedCallback(VulkanFrameRenderedCallback);
    app.RegisterVulkanFrameRenderStartCallback(VulkanFrameRenderStartCallback);
    app.RegisterVulkanGetVerticesCallback(VulkanGetVerticesCallback);
    app.RegisterVulkanGetComputeBuffersCallback(VulkanGetComputeBuffersCallback);

#if RUN_TEST_ID == 2
    physicsGPUEngine->AddMolecule(
        PhysicsConfiguration::GasMoleculeDiameter * 1.3f, 
        PhysicsConfiguration::GasMoleculeDiameter * 2.0f, 
        0.0f, 
        -1.0f,
        distributionColor(gen),
        distributionColor(gen),
        distributionColor(gen)
    );
#endif

#if RUN_TEST_ID == 1
    for (unsigned i = 0; i < Physics::PhysicsConfiguration::GasMoleculeStartCount; i++)
    {
        physicsGPUEngine->AddMolecule(
            distributionPosition(gen), 
            distributionPosition(gen), 
            distributionVelocity(gen), 
            distributionVelocity(gen),
            distributionColor(gen),
            distributionColor(gen),
            distributionColor(gen)
        );
    }
#endif

#if RUN_TEST_ID == 0
    physicsGPUEngine->AddMolecule(-0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    physicsGPUEngine->AddMolecule(0.0f, 0.0f, -0.2f, 0.0f, 0.0f, 1.0f, 0.0f);
#endif

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}