#include "Graphics/Vulkan/VulkanBuffer.h"

namespace Hyperion
{
	void VulkanBuffer::init(
		VulkanDevice& device,
		VkDeviceSize bufferSize,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags)
	{
		m_bufferSize = bufferSize;
		m_usageFlags = usageFlags;
		m_memoryPropertyFlags = memoryPropertyFlags;

		device.createBuffer(m_bufferSize, m_usageFlags, m_memoryPropertyFlags, m_buffer, m_bufferMemory);
	}

	void VulkanBuffer::cleanup(const VulkanDevice& device) const
	{
		vkDestroyBuffer(device.device(), m_buffer, VK_NULL_HANDLE);
		vkFreeMemory(device.device(), m_bufferMemory, VK_NULL_HANDLE);
	}

	void VulkanBuffer::setData(const VulkanDevice& device, const void* data, VkDeviceSize size, VkDeviceSize offset) const
	{
		void* tempData;
		vkMapMemory(device.device(), m_bufferMemory, offset, size, 0, &tempData);
		memcpy(tempData, data, (size_t)size);
		vkUnmapMemory(device.device(), m_bufferMemory);
	}
}