#include "Graphics/Gui/ImGuiOverlay.h"

#include "Core/Application.h"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

namespace Hyperion
{
	static void check_vk_result(VkResult err)
	{
		if (err == VK_SUCCESS)
			return;
		throw std::runtime_error("[vulkan] Error: VkResult = " + err);
	}

	void ImGuiOverlay::init(InitInfo initInfo)
	{
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


		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)initInfo.Window->getNativeWindow(), true);
		ImGui_ImplVulkan_InitInfo vulkanInitInfo = {};
		vulkanInitInfo.Instance = initInfo.Instance;
		vulkanInitInfo.PhysicalDevice = initInfo.PhysicalDevice;
		vulkanInitInfo.Device = initInfo.Device;
		vulkanInitInfo.QueueFamily = initInfo.QueueFamily;
		vulkanInitInfo.Queue = initInfo.Queue;
		vulkanInitInfo.PipelineCache = initInfo.PipelineCache;
		vulkanInitInfo.DescriptorPool = initInfo.DescriptorPool;
		vulkanInitInfo.MinImageCount = 2;
		vulkanInitInfo.ImageCount = initInfo.ImageCount;
		vulkanInitInfo.Allocator = initInfo.Allocator;
		vulkanInitInfo.RenderPass = initInfo.RenderPass;
		vulkanInitInfo.Subpass = 0;
		vulkanInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		vulkanInitInfo.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&vulkanInitInfo);
	}

	void ImGuiOverlay::cleanup()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiOverlay::begin()
	{
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

		// Set delta time (TODO: use a proper timer)
		static double lastTime = 0.0;
		double currentTime = glfwGetTime();
		io.DeltaTime = lastTime > 0.0 ? (float)(currentTime - lastTime) : (float)(1.0f / 60.0f);
		lastTime = currentTime;

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiOverlay::end(VkCommandBuffer commandBuffer)
	{
		// Rendering
		ImGui::Render();

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		// Update and Render additional Platform Windows
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}