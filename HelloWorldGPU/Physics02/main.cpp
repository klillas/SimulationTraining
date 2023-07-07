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

#include <sstream>
#include <iomanip>
#include "Vulkan/VulkanInit.h"
#include "./Debug/DebugOutput.h"
#include "./Physics/Engine/IPhysicsEngine.h"
#include "./Physics/Engine/PhysicsEngine.h"
#include "./Physics/PhysicsConfiguration.h"
#include <random>

using namespace Physics::RigidObjects;

Physics::Engine::IPhysicsEngine* engine = new Physics::Engine::PhysicsEngine();

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

        if (framesPerSecond > 55)
        {
            unsigned newMoleculeSize = engine->GasMoleculeCount() * 0.05f + 1;
            for (unsigned i = 0; i < newMoleculeSize; i++)
            {
                GasMolecules::GasMolecule* gasMolecule = new GasMolecules::GasMolecule();
                gasMolecule->position = glm::vec2(distributionPosition(gen), distributionPosition(gen));
                gasMolecule->color = glm::vec3(distributionColor(gen), distributionColor(gen), distributionColor(gen));
                gasMolecule->velocity = glm::vec2(distributionVelocity(gen), distributionVelocity(gen));

                engine->AddGasMolecule(gasMolecule);
            }
        }

        std::string str2 = "Molecules: ";
        // Convert float to string
        std::stringstream ss2;
        ss2 << std::fixed << std::setprecision(1) << engine->GasMoleculeCount();
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
    engine->PhysicsTick(0.01f);
}

void VulkanGetVerticesCallback(std::vector<VulkanInit::Vertex>* verticesBuffer)
{
    verticesBuffer->clear();
    std::vector<VulkanInit::Vertex> newVerts = engine->GetVertices();
    verticesBuffer->insert(verticesBuffer->end(), newVerts.begin(), newVerts.end());
}

int main() {
    VulkanInit app;
    app.RegisterVulkanFrameRenderedCallback(VulkanFrameRenderedCallback);
    app.RegisterVulkanFrameRenderStartCallback(VulkanFrameRenderStartCallback);
    app.RegisterVulkanGetVerticesCallback(VulkanGetVerticesCallback);

    for (unsigned i = 0; i < Physics::PhysicsConfiguration::GasMoleculeCount; i++)
    {
        GasMolecules::GasMolecule* gasMolecule = new GasMolecules::GasMolecule();
        gasMolecule->position = glm::vec2(distributionPosition(gen), distributionPosition(gen));
        gasMolecule->color = glm::vec3(distributionColor(gen), distributionColor(gen), distributionColor(gen));
        gasMolecule->velocity = glm::vec2(distributionVelocity(gen), distributionVelocity(gen));

        engine->AddGasMolecule(gasMolecule);
    }

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}