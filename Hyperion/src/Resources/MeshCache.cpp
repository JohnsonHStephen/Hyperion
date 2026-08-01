#include "Resources/MeshCache.h"

namespace Hyperion
{
	MeshCache::MeshCache()
		: m_meshes()
	{
	}

	MeshCache::~MeshCache()
	{
		if (m_meshes.size() > 0)
		{
			throw std::runtime_error("MeshCache must be cleared before destruction");
		}
	}

	void MeshCache::init(const VulkanDevice& device)
	{
		clear(device);
	}

	void MeshCache::cleanup(const VulkanDevice& device)
	{
		clear(device);
	}

	MeshId MeshCache::addMesh(VulkanDevice& device, const CPUMesh& cpuMesh)
	{
		GPUMesh gpuMesh{};

		gpuMesh.numVerticies = cpuMesh.verticies.size();
		gpuMesh.numIndicies = cpuMesh.indicies.size();

		uploadMeshToGPU(device, gpuMesh, cpuMesh);

		m_meshes.push_back(gpuMesh);
		return m_meshes.size() - 1;
	}

	bool MeshCache::hasMesh(MeshId meshId) const
	{
		return meshId < m_meshes.size();
	}

	const GPUMesh& MeshCache::getMesh(MeshId meshId) const
	{
		if (meshId >= m_meshes.size())
			throw std::runtime_error("Invalid mesh ID");
		return m_meshes[meshId];
	}

	void MeshCache::clear(const VulkanDevice& device)
	{
		for (auto& mesh : m_meshes)
		{
			mesh.vertexBuffer.cleanup(device);
			mesh.indexBuffer.cleanup(device);
		}

		m_meshes.clear();
	}

	void MeshCache::uploadMeshToGPU(VulkanDevice& device, GPUMesh& gpuMesh, const CPUMesh& cpuMesh)
	{
		if (gpuMesh.numVerticies <= 0)
			throw std::runtime_error("Verticies must be initialized before buffers are created");

		if (gpuMesh.numIndicies <= 0)
			throw std::runtime_error("Indicies must be initialized before buffers are created");

		VkDeviceSize vertexBufferSize = gpuMesh.numVerticies * sizeof(CPUMesh::Vertex);
		VkDeviceSize indexBufferSize = gpuMesh.numIndicies * sizeof(uint32_t);

		VulkanBuffer stagingBuffer{};
		stagingBuffer.init(
			device,
			vertexBufferSize + indexBufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.setData(device, cpuMesh.verticies.data(), vertexBufferSize);
		stagingBuffer.setData(device, cpuMesh.indicies.data(), indexBufferSize, vertexBufferSize);

		gpuMesh.vertexBuffer.init(
			device,
			vertexBufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		gpuMesh.indexBuffer.init(
			device,
			indexBufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		device.copyBuffer(stagingBuffer.getInternalBuffer(), gpuMesh.vertexBuffer.getInternalBuffer(), vertexBufferSize);
		device.copyBuffer(stagingBuffer.getInternalBuffer(), gpuMesh.indexBuffer.getInternalBuffer(), indexBufferSize, vertexBufferSize);

		stagingBuffer.cleanup(device);
	}
}