#include "SandboxGame.h"

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

void SandboxGame::init()
{}

void SandboxGame::cleanup()
{}

void SandboxGame::onUpdate(float dt)
{}

void SandboxGame::onEvent(Hyperion::Event & e)
{}

void SandboxGame::onOverlay()
{}

void SandboxGame::onRender()
{}
