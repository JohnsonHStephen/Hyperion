#pragma once

#include "Core/HyperionPCH.h"

#include <Vulkan/vulkan.h>

#include "Graphics/Vulkan/VulkanDevice.h"

namespace Hyperion
{
	enum class ShaderDataType
	{
		e_None = 0,
		e_Float,
		e_Float2,
		e_Float3,
		e_Float4,
		e_Int,
		e_Int2,
		e_Int3,
		e_Int4,
		e_Bool,
	};

	static uint32_t shaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::e_Float:		return 4;
		case ShaderDataType::e_Float2:		return 4 * 2;
		case ShaderDataType::e_Float3:		return 4 * 3;
		case ShaderDataType::e_Float4:		return 4 * 4;
		case ShaderDataType::e_Int:			return 4;
		case ShaderDataType::e_Int2:		return 4 * 2;
		case ShaderDataType::e_Int3:		return 4 * 3;
		case ShaderDataType::e_Int4:		return 4 * 4;
		case ShaderDataType::e_Bool:		return 1;
		}

		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		size_t Offset;
		uint32_t Size;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			:Name(name), Type(type), Size(shaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			calculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& getElements() const { return m_Elements; }
		inline uint32_t getStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void calculateOffsetAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VulkanBuffer
	{
	public:
		VulkanBuffer();
		~VulkanBuffer();

		void init(
			VulkanDevice* device,
			BufferLayout bufferLayout,
			uint32_t instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags);
		void cleanup() const;

		VkBuffer getInternalBuffer() const { return m_buffer; };
		uint32_t getInstanceCount() const { return m_instanceCount; }
		BufferLayout getBufferLayout() const { return m_bufferLayout; }

		void setData(const void* data, uint32_t size) const;

	private:
		VulkanDevice* m_device = nullptr;
		VkBuffer				m_buffer = VK_NULL_HANDLE;
		VkDeviceMemory			m_bufferMemory = VK_NULL_HANDLE;

		VkDeviceSize			m_bufferSize = 0;
		uint32_t				m_instanceCount = 0;
		VkDeviceSize			m_instanceSize = 0;
		VkDeviceSize			m_alignmentSize = 0;
		VkBufferUsageFlags		m_usageFlags = 0;
		VkMemoryPropertyFlags	m_memoryPropertyFlags = 0;

		BufferLayout			m_bufferLayout{};
	};
}