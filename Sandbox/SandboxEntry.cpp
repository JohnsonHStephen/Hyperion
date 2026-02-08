#include "Core/Hyperion.h"
#include "Core/HyperionPCH.h"
#include "Core/Main.h"

#include "SandboxListener.h"

class SandboxApp: public Hyperion::Application {
public:
	SandboxApp(ApplicationInfo appInfo): Application(appInfo) {
		pushListener(new SandboxListener());
	}

	~SandboxApp() {

	}

};

Hyperion::Application* Hyperion::createApplication() {
	ApplicationInfo appInfo{};
	appInfo.appName = "Test Name";
	appInfo.appVersionVariant = 0;
	appInfo.appVersionMajor = 0;
	appInfo.appVersionMinor = 1;
	appInfo.appVersionPatch = 0;

	return new SandboxApp(appInfo);
}