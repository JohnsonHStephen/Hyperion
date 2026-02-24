#include "Core/Hyperion.h"

class SandboxGame: public Hyperion::Game
{
public:
	virtual Hyperion::ApplicationInfo getAppInfo() override;

	virtual void init() override;
	virtual void cleanup() override;

	virtual void onUpdate(float dt) override;
	virtual void onEvent(Hyperion::Event& e) override;

	virtual void onOverlay() override;
	virtual void onRender() override;

	virtual Hyperion::World& getWorld() override { return m_world; }

private:
	Hyperion::World m_world{};
};