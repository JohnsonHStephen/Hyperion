#pragma once

#include "Core/HyperionPCH.h"
#include "Core/Game.h"

#include "Events/Event.h"
#include "Events/WindowEvent.h"

#include "Graphics/Gui/ImGuiOverlay.h"
#include "Graphics/Gui/Window.h"

#include "Graphics/Pipelines/MeshPipeline.h"

#include "Resources/MeshCache.h"

#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanRenderer.h"

namespace Hyperion
{
	class Application
	{
	public:
		Application(const ApplicationInfo& appInfo);
		~Application();

		void run(Game* game);
		void close();

		void onEvent(Event& e);

		inline const Window* getWindow() const { return m_window.get(); }
		inline Window* getWindow() { return m_window.get(); }

		inline const VulkanDevice* getVulkanDevice() const { return &m_device; }
		inline VulkanDevice* getVulkanDevice() { return &m_device; }

	private:
		bool onWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_window = nullptr;
		VulkanDevice m_device{};
		VulkanRenderer m_renderer{};

		Game* m_game = nullptr;

		ImGuiOverlay m_overlay{};

		MeshPipeline m_meshPipeline{};
		MeshCache m_meshCache{};

		bool m_running = false;
	};
} // namespace Hyperion