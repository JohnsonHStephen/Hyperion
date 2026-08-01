#include "Resources/MeshLoader.h"

namespace Hyperion
{
	CPUMesh MeshLoader::loadCustomData(const std::vector<CPUMesh::Vertex>& verticies, const std::vector<uint32_t>& indicies)
	{
		CPUMesh mesh;

		const uint32_t numVerticies = verticies.size();
		mesh.verticies.resize(numVerticies);

		for (int i = 0; i < numVerticies; i++)
		{
			mesh.verticies[i] = verticies[i];
		}

		const uint32_t numIndicies = indicies.size();
		mesh.indicies.resize(numIndicies);

		for (int i = 0; i < numIndicies; i++)
		{
			mesh.indicies[i] = indicies[i];
		}

		return mesh;
	}
}