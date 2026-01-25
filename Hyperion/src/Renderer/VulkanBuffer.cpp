#include "Renderer/VulkanBuffer.h"

namespace Hyperion
{
	VulkanBuffer::VulkanBuffer() {}

	VulkanBuffer::~VulkanBuffer() {}

	void VulkanBuffer::Init(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDevice& device, VkPhysicalDevice& physicalDevice) {
		m_device = device;
		m_physicalDevice = physicalDevice;
		m_size = size;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_device, m_buffer, m_bufferMemory, 0);
	}

	void VulkanBuffer::Cleanup() const {
		vkDestroyBuffer(m_device, m_buffer, nullptr);
		vkFreeMemory(m_device, m_bufferMemory, nullptr);
	}

	void VulkanBuffer::SetData(const void* data, uint32_t size) const {
		void* tempData;
		vkMapMemory(m_device, m_bufferMemory, 0, m_size, 0, &tempData);
		memcpy(tempData, data, (size_t)size);
		vkUnmapMemory(m_device, m_bufferMemory);
	}

	uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
}