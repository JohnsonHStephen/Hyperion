#pragma once

#include "Core/HyperionPCH.h"

#include <Vulkan/vulkan.h>

#include "Graphics/Vulkan/VulkanDevice.h"

namespace Hyperion
{
	class VulkanBuffer
	{
	public:
		void init(
			VulkanDevice* device,
			VkDeviceSize bufferSize,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags);
		void cleanup() const;

		VkBuffer getInternalBuffer() const { return m_buffer; };

		void setData(const void* data, VkDeviceSize size, VkDeviceSize offset = 0) const;

	private:
		VulkanDevice* m_device = nullptr;
		VkBuffer				m_buffer = VK_NULL_HANDLE;
		VkDeviceMemory			m_bufferMemory = VK_NULL_HANDLE;

		VkDeviceSize			m_bufferSize = 0;
		VkBufferUsageFlags		m_usageFlags = 0;
		VkMemoryPropertyFlags	m_memoryPropertyFlags = 0;
	};
}