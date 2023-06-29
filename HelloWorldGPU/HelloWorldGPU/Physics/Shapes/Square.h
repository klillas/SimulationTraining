#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Vulkan/VulkanInit.h"
#include "IShape.h"

namespace Physics::Shapes
{
	class Square : public IShape
	{
	public:
		glm::vec2 m_position;
		glm::vec3 m_color;
		float m_size;
		float m_rotation;

		Square(float size, glm::vec2 position, float rotation);

		Square();

		// Inherited via IShape
		glm::vec2 GetPosition();
		void SetPosition(glm::vec2 newPos);
		float GetRotation();
		void SetRotation(float newRotation);
		glm::vec3 GetColor();
		void SetColor(glm::vec3 newColor);
		std::vector<VulkanInit::Vertex> GetVertices();
	};
};

