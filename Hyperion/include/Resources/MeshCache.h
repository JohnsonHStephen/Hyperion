#pragma once

#include "Resources/Mesh.h"
#include "Graphics/Vulkan/VulkanDevice.h"

namespace Hyperion
{
	using MeshId = std::size_t;
	static const std::size_t NULL_MESH_ID = std::numeric_limits<std::size_t>::max();

	class MeshCache
	{
	public:
		MeshCache();
		~MeshCache();

		void init(const VulkanDevice& device);
		void cleanup(const VulkanDevice& device);

		MeshId addMesh(VulkanDevice& device, const CPUMesh& cpuMesh);
		bool hasMesh(MeshId meshId) const;
		const GPUMesh& getMesh(MeshId meshId) const;

		void clear(const VulkanDevice& device);

	private:
		std::vector<GPUMesh> m_meshes;

	private:
		void uploadMeshToGPU(VulkanDevice& device, GPUMesh& gpuMesh, const CPUMesh& cpuMesh);
	};
}