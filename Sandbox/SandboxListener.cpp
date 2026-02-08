#include "SandboxListener.h"

#include "imgui.h"

SandboxListener::SandboxListener() {}
SandboxListener::~SandboxListener() {}
void SandboxListener::onAttach() {}
void SandboxListener::onDetach() {}
void SandboxListener::onFrameStart(int ts) {}
void SandboxListener::onFrameEnd(int ts) {}

void SandboxListener::onOverlay()
{
	//ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());


	ImGui::Begin("Test");

	ImGui::Text("Tesssst");

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");
	ImGui::PopStyleVar();
	ImGui::End();
}