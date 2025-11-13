#include "SandboxLayer.h"

#include "imgui.h"

namespace Hyperion
{
	SandboxLayer::SandboxLayer() {}
	SandboxLayer::~SandboxLayer() {}
	void SandboxLayer::onAttach() {}
	void SandboxLayer::OnDetach() {}
	void SandboxLayer::OnFrameStart(int ts) {}
	void SandboxLayer::OnFrameEnd(int ts) {}
	void SandboxLayer::OnOverlay() {
		/*ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());


		ImGui::Begin("Test");

		ImGui::Text("Tesssst");

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImGui::PopStyleVar();
		ImGui::End();*/
	}
}