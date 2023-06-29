#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Vulkan/VulkanInit.h"

namespace Physics::Shapes
{
	class IShape
	{
		virtual glm::vec2 GetPosition() = 0;

		virtual void SetPosition(glm::vec2 newPos) = 0;

		virtual float GetRotation() = 0;

		virtual void SetRotation(float newRotation) = 0;

		virtual glm::vec3 GetColor() = 0;

		virtual void SetColor(glm::vec3 newColor) = 0;

		virtual std::vector<VulkanInit::Vertex> GetVertices() = 0;
	};
};

