#pragma once

#include "GUI/Layer.h"

namespace Hyperion
{
	class ImGuiLayer : private Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;

		void Begin();
		void End();

		void SetEventBlocking(bool block) { m_EventBlocking = block; }

	private:
		bool m_EventBlocking = true;
	};
}
