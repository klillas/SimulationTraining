#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Vulkan/VulkanInit.h"
#include "IShape.h"

namespace Physics::Shapes
{
	class GasMolecule : public IShape
	{
	public:
		glm::vec2 m_position;
		glm::vec3 m_color;

		GasMolecule(glm::vec2 position);

		GasMolecule();

		// Inherited via IShape
		glm::vec2 GetPosition();
		void SetPosition(glm::vec2 newPos);
		glm::vec3 GetColor();
		void SetColor(glm::vec3 newColor);
		std::vector<VulkanInit::Vertex> GetVertices();
		bool Intersects(IShape* shape);
		void AddPosition(glm::vec2 positionIncrement);
	};
};

