#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Graphics/Vulkan/VulkanBuffer.h"

namespace Hyperion
{
	struct CPUMesh
	{
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 colour;
		};

		std::vector<Vertex> verticies;
		std::vector<uint32_t> indicies;

		const std::string name;
	};

	struct GPUMesh
	{
		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;

		uint32_t numVerticies{ 0 };
		uint32_t numIndicies{ 0 };
	};
}