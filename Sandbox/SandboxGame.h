#include "Core/Hyperion.h"

#include "World/Camera.h"
#include "World/Entity.h"

class SandboxGame: public Hyperion::Game
{
public:
	virtual Hyperion::ApplicationInfo getAppInfo() override;

	virtual void init(Hyperion::GameContext context) override;
	virtual void cleanup() override;

	virtual void onUpdate(float dt) override;
	virtual void onEvent(Hyperion::Event& e) override;

	virtual void onOverlay() override;
	virtual void onRender() override;

	virtual Hyperion::World& getWorld() override { return m_world; }

	virtual std::optional<Hyperion::Entity> getPrimaryCameraEntity() override;

private:
	Hyperion::World m_world{};
	Hyperion::Entity m_cubeEntity;
	Hyperion::Entity m_cameraEntity;

	glm::vec3 m_viewDirection{ 0.0f, 0.0f, 0.0f };
};