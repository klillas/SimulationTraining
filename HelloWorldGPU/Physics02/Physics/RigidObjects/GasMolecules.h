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
			
			/// <summary>
			/// Next item in the SpaceGridMolecules cell
			/// </summary>
			GasMolecule* nextItem;
			/// <summary>
			/// Previous item in the SpaceGridMolecules cell
			/// </summary>
			GasMolecule* previousItem;
			/// <summary>
			/// x index of the current Cell which hosts this GasMolecule
			/// </summary>
			unsigned cellIndexX;
			/// <summary>
			/// y index of the current Cell which hosts this GasMolecule
			/// </summary>
			unsigned cellIndexY;
		};

	public:
		static void GetVertices(GasMolecule* gasMolecule, std::vector<VulkanInit::Vertex>* vertexVector);

		static bool Intersects(GasMolecule* moleculeOne, GasMolecule* moleculeTwo);

		static void PhysicsTick(float timeDelta, GasMolecule* gasMolecule);
	};
};

