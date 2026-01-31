#include "Renderer/VulkanBuffer.h"

namespace Hyperion
{
	VulkanBuffer::VulkanBuffer() {}

	VulkanBuffer::~VulkanBuffer() {}

	void VulkanBuffer::init(
		VulkanDevice* device,
		BufferLayout bufferLayout,
		uint32_t instanceCount,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags)
	{
		m_device = device;
		m_bufferLayout = bufferLayout;
		m_instanceSize = bufferLayout.getStride();
		m_instanceCount = instanceCount;
		m_usageFlags = usageFlags;
		m_memoryPropertyFlags = memoryPropertyFlags;

		m_alignmentSize = m_instanceSize; // TODO: won't always be true
		m_bufferSize = m_alignmentSize * m_instanceCount;

		m_device->createBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_bufferMemory);
	}

	void VulkanBuffer::cleanup() const
	{
		vkDestroyBuffer(m_device->device(), m_buffer, nullptr);
		vkFreeMemory(m_device->device(), m_bufferMemory, nullptr);
	}

	void VulkanBuffer::setData(const void* data, uint32_t size) const
	{
		void* tempData;
		vkMapMemory(m_device->device(), m_bufferMemory, 0, m_bufferSize, 0, &tempData);
		memcpy(tempData, data, (size_t)size);
		vkUnmapMemory(m_device->device(), m_bufferMemory);
	}
}