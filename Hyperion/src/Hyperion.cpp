// Hyperion.cpp : Defines the entry point for the application.
//

#include "Hyperion.h"

#include <imgui.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "ImGuiLayer.h"
#include "WindowsWindow.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
	std::unique_ptr<Hyperion::WindowsWindow> window = std::make_unique<Hyperion::WindowsWindow>(Hyperion::WindowsWindow::WindowProps());

	window->SetEventCallback(key_callback);

	Hyperion::ImGuiLayer imguiLayer = Hyperion::ImGuiLayer();
	imguiLayer.OnAttach((GLFWwindow*)window->GetNativeWindow());

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose((GLFWwindow*)window->GetNativeWindow())) {
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		imguiLayer.Begin(); // Start the ImGui frame
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)


			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);
			ImGui::End();
		}
		imguiLayer.End(); // End the ImGui frame


		window->OnUpdate();
	}

	imguiLayer.OnDetach();

	exit(EXIT_SUCCESS);
}