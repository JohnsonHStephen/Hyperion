#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Hyperion
{
	class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach(GLFWwindow* window);
		void OnDetach();

		void Begin();
		void End();

		void SetEventBlocking(bool block) { m_EventBlocking = block; }

	private:
		bool m_EventBlocking = true;
	};
}
