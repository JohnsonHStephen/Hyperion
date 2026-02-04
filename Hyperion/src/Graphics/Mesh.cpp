#include "Graphics/Mesh.h"

namespace Hyperion
{
	Mesh::Mesh(const std::string name, VulkanDevice* device):
		m_device(device), m_name(name), m_vertexBuffer(), m_indexBuffer()
	{
		m_verticies = std::vector<Vertex>();
		m_indicies = std::vector<uint32_t>();
	}

	Mesh::~Mesh()
	{
		if (m_numVerticies > 0)
			m_vertexBuffer.cleanup();
		if (m_numIndicies > 0)
			m_indexBuffer.cleanup();
	}

	void Mesh::loadCustomData(const std::vector<Vertex>& verticies, const std::vector<uint32_t>& indicies)
	{
		m_numVerticies = verticies.size();
		m_verticies.resize(m_numVerticies);

		for (int i = 0; i < m_numVerticies; i++)
		{
			m_verticies[i] = verticies[i];
		}

		m_numIndicies = indicies.size();
		m_indicies.resize(m_numIndicies);

		for (int i = 0; i < m_numIndicies; i++)
		{
			m_indicies[i] = indicies[i];
		}

		createBuffers();
	}

	void Mesh::createBuffers()
	{
		if (m_numVerticies <= 0)
			throw std::runtime_error("Verticies must be initialized before buffers are created");

		if (m_numIndicies <= 0)
			throw std::runtime_error("Indicies must be initialized before buffers are created");

		VkDeviceSize vertexBufferSize = m_numVerticies * sizeof(Vertex);
		VkDeviceSize indexBufferSize = m_numIndicies * sizeof(uint32_t);

		VulkanBuffer stagingBuffer{};
		stagingBuffer.init(
			m_device,
			vertexBufferSize + indexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.setData(m_verticies.data(), vertexBufferSize);
		stagingBuffer.setData(m_indicies.data(), indexBufferSize, vertexBufferSize);

		m_vertexBuffer.init(
			m_device,
			vertexBufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_indexBuffer.init(
			m_device,
			indexBufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_device->copyBuffer(stagingBuffer.getInternalBuffer(), m_vertexBuffer.getInternalBuffer(), vertexBufferSize);
		m_device->copyBuffer(stagingBuffer.getInternalBuffer(), m_indexBuffer.getInternalBuffer(), indexBufferSize, vertexBufferSize);

		stagingBuffer.cleanup();
	}
}