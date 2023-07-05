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

void VulkanFrameRenderedCallback(unsigned frametime_ms)
{
    static unsigned frametime_total = 0;
    static unsigned framecounter = 0;
    static Debug::DebugOutput* debug = Debug::DebugOutput::GetInstance();

    frametime_total += frametime_ms;
    framecounter++;

    if (frametime_total >= 1000)
    {
        std::string str = "Frames per second: ";
        // Convert float to string
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << (framecounter * 1000.0f / frametime_total);
        std::string floatString = ss.str();

        // Concatenate the string and float string
        std::string result = str + floatString;
        debug->Print(&result);

        framecounter = 0;
        frametime_total = 0;
    }
}

int main() {
    VulkanInit app;
    app.RegisterVulkanFrameRenderedCallback(VulkanFrameRenderedCallback);

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}