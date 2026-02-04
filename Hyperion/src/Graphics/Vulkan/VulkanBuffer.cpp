#include "Graphics/Vulkan/VulkanBuffer.h"

namespace Hyperion
{
	void VulkanBuffer::init(
		VulkanDevice* device,
		VkDeviceSize bufferSize,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags)
	{
		m_device = device;
		m_bufferSize = bufferSize;
		m_usageFlags = usageFlags;
		m_memoryPropertyFlags = memoryPropertyFlags;

		m_device->createBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_bufferMemory);
	}

	void VulkanBuffer::cleanup() const
	{
		vkDestroyBuffer(m_device->device(), m_buffer, nullptr);
		vkFreeMemory(m_device->device(), m_bufferMemory, nullptr);
	}

	void VulkanBuffer::setData(const void* data, VkDeviceSize size, VkDeviceSize offset) const
	{
		void* tempData;
		vkMapMemory(m_device->device(), m_bufferMemory, offset, size, 0, &tempData);
		memcpy(tempData, data, (size_t)size);
		vkUnmapMemory(m_device->device(), m_bufferMemory);
	}
}