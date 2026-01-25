#pragma once

#include "Core/HyperionPCH.h"

#include <Vulkan/vulkan.h>

namespace Hyperion
{

	enum class ShaderDataType {
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

	static uint32_t ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
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

	static VkFormat ShaderDataTypeToVulkan(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::e_Float:		return VK_FORMAT_R32_SFLOAT;
		case ShaderDataType::e_Float2:		return VK_FORMAT_R32G32_SFLOAT;
		case ShaderDataType::e_Float3:		return VK_FORMAT_R32G32B32_SFLOAT;
		case ShaderDataType::e_Float4:		return VK_FORMAT_R32G32B32A32_SFLOAT;
		case ShaderDataType::e_Int:			return VK_FORMAT_R32_SINT;
		case ShaderDataType::e_Int2:		return VK_FORMAT_R32G32_SINT;
		case ShaderDataType::e_Int3:		return VK_FORMAT_R32G32B32_SINT;
		case ShaderDataType::e_Int4:		return VK_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::e_Bool:		return VK_FORMAT_R8_UINT;
		}

		return VK_FORMAT_UNDEFINED;
	}

	struct BufferElement {
		std::string Name;
		ShaderDataType Type;
		size_t Offset;
		uint32_t Size;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			:Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

		uint32_t GetComponentCount() const {
			switch (Type) {
			case ShaderDataType::e_Float:	return 1;
			case ShaderDataType::e_Float2:	return 2;
			case ShaderDataType::e_Float3:	return 3;
			case ShaderDataType::e_Float4:	return 4;
			case ShaderDataType::e_Int:		return 1;
			case ShaderDataType::e_Int2:	return 2;
			case ShaderDataType::e_Int3:	return 3;
			case ShaderDataType::e_Int4:	return 4;
			case ShaderDataType::e_Bool:	return 1;
			}

			return 0;
		}

		uint32_t GetLocationSize() const {
			switch (Type) {
			case ShaderDataType::e_Float:	return 1;
			case ShaderDataType::e_Float2:	return 1;
			case ShaderDataType::e_Float3:	return 1;
			case ShaderDataType::e_Float4:	return 1;
			case ShaderDataType::e_Int:		return 1;
			case ShaderDataType::e_Int2:	return 1;
			case ShaderDataType::e_Int3:	return 1;
			case ShaderDataType::e_Int4:	return 1;
			case ShaderDataType::e_Bool:	return 1;
			}

			return 0;
		}
	};

	class BufferLayout {
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements) {
			CalculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetAndStride() {
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements) {
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VulkanVertexBuffer {
	public:
		VulkanVertexBuffer();
		~VulkanVertexBuffer();

		void Init(uint32_t size, VkDevice device, VkPhysicalDevice physicalDevice);
		void Cleanup() const;

		VkBuffer GetInternalBuffer() const { return m_vertexBuffer; };

		inline const BufferLayout& GetLayout() const { return m_layout; }
		inline void SetLayout(const BufferLayout& layout) { m_layout = layout; }

		void SetData(const void* data, uint32_t size) const;

	private:
		VkDevice m_device = VK_NULL_HANDLE;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		BufferLayout m_layout;
		uint32_t m_size = 0;

		VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_vertexBufferMemory = VK_NULL_HANDLE;

	private:
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};
}