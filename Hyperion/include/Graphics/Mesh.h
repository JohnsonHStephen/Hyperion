#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Graphics/Vulkan/VulkanBuffer.h"

namespace Hyperion
{
	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec2 position;
			glm::vec3 colour;
		};

	public:
		Mesh(const std::string name, VulkanDevice* device);
		~Mesh();

		void loadCustomData(const std::vector<Vertex>& verticies, const std::vector<uint32_t>& indicies);

		uint32_t getVertexCount() const { return m_numVerticies; }
		VkBuffer getVertexBuffer() const { return m_vertexBuffer.getInternalBuffer(); }
		uint32_t getIndexCount() const { return m_numIndicies; }
		VkBuffer getIndexBuffer() const { return m_indexBuffer.getInternalBuffer(); }

	private:
		void createBuffers();

	private:
		VulkanDevice* m_device;

		VulkanBuffer m_vertexBuffer;
		VulkanBuffer m_indexBuffer;

		std::vector<Vertex> m_verticies;
		std::vector<uint32_t> m_indicies;

		const std::string m_name;

		uint32_t m_numVerticies{ 0 };
		uint32_t m_numIndicies{ 0 };
	};
}