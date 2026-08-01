#pragma once

#include "Resources/Mesh.h"

namespace Hyperion
{
	namespace MeshLoader
	{
		CPUMesh loadCustomData(const std::vector<CPUMesh::Vertex>& verticies, const std::vector<uint32_t>& indicies);
	}
}