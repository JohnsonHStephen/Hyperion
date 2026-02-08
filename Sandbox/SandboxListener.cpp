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
	ImGui::Begin("Test");

	ImGui::Text("Tesssst");

	ImGui::End();
}