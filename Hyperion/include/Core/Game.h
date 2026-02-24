#pragma once

#include "Events/Event.h"

#include "World/World.h"

namespace Hyperion
{
	struct ApplicationInfo
	{
		unsigned short appVersionVariant;
		unsigned short appVersionMajor;
		unsigned short appVersionMinor;
		unsigned short appVersionPatch;

		std::string appName;
	};

	class Game
	{
	public:
		virtual ApplicationInfo getAppInfo() = 0;

		virtual void init() = 0;
		virtual void cleanup() = 0;

		virtual void onUpdate(float dt) = 0;
		virtual void onEvent(Event& e) = 0;

		virtual void onOverlay() = 0;
		virtual void onRender() = 0;

		virtual World& getWorld() = 0;
	};
}