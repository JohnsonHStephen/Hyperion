#include "Core/Hyperion.h"
#include "Core/HyperionPCH.h"
#include "Core/Main.h"

#include "SandboxLayer.h"

namespace Hyperion
{
	class SandboxApp: public Application {
	public:
		SandboxApp(): Application() {
			PushLayer(new SandboxLayer());
		}

		~SandboxApp() {

		}

	};

	Application* CreateApplication() {
		return new SandboxApp();
	}
}