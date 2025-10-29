#include "Core/Hyperion.h"
#include "Core/HyperionPCH.h"
#include "Core/Main.h"

#include "SandboxLayer.h"

namespace Hyperion
{
	class SandboxApp: public Application {
	public:
		SandboxApp(ApplicationInfo appInfo): Application(appInfo) {
			PushLayer(new SandboxLayer());
		}

		~SandboxApp() {

		}

	};

	Application* CreateApplication() {
		ApplicationInfo appInfo{};
		appInfo.appName = "Test Name";
		appInfo.appVersionVariant = 0;
		appInfo.appVersionMajor = 0;
		appInfo.appVersionMinor = 1;
		appInfo.appVersionPatch = 0;

		return new SandboxApp(appInfo);
	}
}