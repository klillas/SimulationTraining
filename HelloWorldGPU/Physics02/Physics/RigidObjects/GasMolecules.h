#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <Vulkan/VulkanInit.h>

namespace Physics::RigidObjects
{
	class GasMolecules
	{
	public:
		struct GasMolecule
		{
			glm::vec2 position;
			glm::vec3 color;
			glm::vec2 velocity;
		};

	public:
		static void GetVertices(GasMolecule* gasMolecule, std::vector<VulkanInit::Vertex>* vertexVector);

		static bool Intersects(GasMolecule* moleculeOne, GasMolecule* moleculeTwo);

		static void PhysicsTick(float timeDelta, GasMolecule* gasMolecule);
	};
};

