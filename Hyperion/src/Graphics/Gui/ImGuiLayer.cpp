#include "Graphics/Gui/ImGuiLayer.h"

#include "Core/Application.h"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"

namespace Hyperion
{
	Hyperion::ImGuiLayer::ImGuiLayer() {}
	ImGuiLayer::~ImGuiLayer() {}

	void ImGuiLayer::OnAttach() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		//ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::get()->getWindow()->getNativeWindow(), true);
		/*ImGui_ImplVulkan_InitInfo init_info = {};
		ImGui_ImplVulkan_Init(&init_info);

		m_WindowData.Surface = Application::Get()->GetVulkanContext()->GetSurface();
		const VkFormat requestSurfaceImageFormat[] = { Application::Get()->GetVulkanContext()->GetImageFormat() };
		const VkPhysicalDevice physicalDevice = Application::Get()->GetVulkanContext()->GetPhysicalDevice();
		m_WindowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(physicalDevice, m_WindowData.Surface, requestSurfaceImageFormat, 1, VK_COLORSPACE_SRGB_NONLINEAR_KHR);


		m_WindowData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(physicalDevice, m_WindowData.Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));*/
	}

	void ImGuiLayer::OnDetach() {
		//ImGui_ImplVulkan_Shutdown();
		//ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin() {
		//ImGui_ImplVulkan_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		
		// Manually set display size (since we can't use ImGui_ImplGlfw_InitForVulkan)
		ImGuiIO& io = ImGui::GetIO();
		Window* window = Application::get()->getWindow();
		GLFWwindow* glfwWindow = (GLFWwindow*)window->getNativeWindow();
		
		int w = window->getWidth(), h = window->getHeight();
		int display_w, display_h;
		window->getFramebufferPixelSize(display_w, display_h);
		io.DisplaySize = ImVec2((float)w, (float)h);
		if (w > 0 && h > 0)
			io.DisplayFramebufferScale = ImVec2((float)display_w / (float)w, (float)display_h / (float)h);
		
		// Set delta time (simple implementation - you may want to use a proper timer)
		static double lastTime = 0.0;
		double currentTime = glfwGetTime();
		io.DeltaTime = lastTime > 0.0 ? (float)(currentTime - lastTime) : (float)(1.0f / 60.0f);
		lastTime = currentTime;
		
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() {
		// Rendering
		ImGui::Render();

		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}