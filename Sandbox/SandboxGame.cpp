#include "SandboxGame.h"

#include "Resources/MeshLoader.h"

#include "World/Components.h"
#include "World/Entity.h"

Hyperion::ApplicationInfo SandboxGame::getAppInfo()
{
	Hyperion::ApplicationInfo appInfo{};
	appInfo.appName = "Test Name";
	appInfo.appVersionVariant = 0;
	appInfo.appVersionMajor = 0;
	appInfo.appVersionMinor = 2;
	appInfo.appVersionPatch = 0;

	return appInfo;
}

void SandboxGame::init(Hyperion::GameContext context)
{

	std::vector<Hyperion::CPUMesh::Vertex> vertices{
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 0.0f}}
	};

	const Hyperion::CPUMesh cubeMesh = Hyperion::MeshLoader::loadCustomData(vertices, {
		0, 1, 2,	// front face
		2, 1, 3,

		1, 5, 3,	// right face
		3, 5, 7,

		5, 4, 7,	// back face
		7, 4, 6,

		4, 0, 6,	// left face
		6, 0, 2,

		4, 5, 0,	// top face
		0, 5, 1,

		2, 3, 6,	// bottom face
		6, 3, 7 });

	const Hyperion::MeshId cubeMeshId = context.meshCache.addMesh(context.device, cubeMesh);

	cubeEntity = m_world.createEntity("Cube");
	cubeEntity.addComponent<Hyperion::TransformComponent>(glm::vec3{ 0.0f, 0.0f, 2.5f });
	cubeEntity.addComponent<Hyperion::MeshComponent>(cubeMeshId);

	const uuids::uuid cubeId = cubeEntity.getUUID();

}

void SandboxGame::cleanup()
{}

void SandboxGame::onUpdate(float dt)
{
	auto& transformComponent = cubeEntity.getComponent<Hyperion::TransformComponent>();
	transformComponent.rotation.z += 0.001f;
	transformComponent.rotation.y += 0.002f;
	transformComponent.rotation.x += 0.0005f;
}

void SandboxGame::onEvent(Hyperion::Event & e)
{}

void SandboxGame::onOverlay()
{}

void SandboxGame::onRender()
{}
