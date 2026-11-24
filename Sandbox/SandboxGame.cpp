#include "SandboxGame.h"

#include "Resources/MeshLoader.h"

#include "Events/KeyEvent.h"

#include "World/Components.h"
#include "World/Entity.h"

#include <imgui.h>

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

	m_cubeEntity = m_world.createEntity("Cube");
	m_cubeEntity.addComponent<Hyperion::TransformComponent>(glm::vec3{ 0.0f, 0.0f, 2.5f });
	m_cubeEntity.addComponent<Hyperion::MeshComponent>(cubeMeshId);

	m_cameraEntity = m_world.createEntity("Camera");
	m_cameraEntity.addComponent<Hyperion::TransformComponent>(glm::vec3{ 0.0f, 0.0f, 0.0f });
	Hyperion::Camera camera{};
	m_cameraEntity.addComponent<Hyperion::CameraComponent>(camera, true);
}

void SandboxGame::cleanup()
{}

void SandboxGame::onUpdate(float dt)
{
	//auto& transformComponent = m_cubeEntity.getComponent<Hyperion::TransformComponent>();
	//transformComponent.rotation.z += 0.001f;
	//transformComponent.rotation.y += 0.002f;
	//transformComponent.rotation.x += 0.0005f;
}

void SandboxGame::onEvent(Hyperion::Event & e)
{
	if (e.GetEventType() == Hyperion::EventType::KeyPressed)
	{
		auto& keyEvent = static_cast<Hyperion::KeyPressedEvent&>(e);
		auto& transform = m_cameraEntity.getComponent<Hyperion::TransformComponent>();
		switch (keyEvent.GetKeyCode())
		{
		case Hyperion::KeyCode::W:
			transform.rotation.x -= 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::S:
			transform.rotation.x += 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::A:
			transform.rotation.y += 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::D:
			transform.rotation.y -= 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::Q:
			transform.rotation.z += 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::E:
			transform.rotation.z -= 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::Up:
			transform.translation.y += 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::Down:
			transform.translation.y -= 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::Left:
			transform.translation.x += 0.1f;
			e.SetHandled();
			break;
		case Hyperion::KeyCode::Right:
			transform.translation.x -= 0.1f;
			e.SetHandled();
			break;
		}
	}
}

void SandboxGame::onOverlay()
{
	ImGui::Begin("Sandbox Game Overlay");
	ImGui::Text("Hello from Sandbox Game!");
	ImGui::End();
}

void SandboxGame::onRender()
{}

std::optional<Hyperion::Entity> SandboxGame::getPrimaryCameraEntity()
{
	auto view = m_world.getAllEntitiesWith<Hyperion::CameraComponent>();
	for (auto& entity : view)
	{
		Hyperion::CameraComponent& cc = entity.getComponent<Hyperion::CameraComponent>();
		if (cc.primary)
			return entity;
	}

	return std::nullopt;
}
