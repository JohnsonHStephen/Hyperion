#pragma once

namespace Hyperion
{
	class Layer {
	public:
		Layer() {};
		virtual ~Layer() {};

		virtual void OnAttach() {};
		virtual void OnDetach() {};

		virtual void OnFrameStart(int ts) {};
		virtual void OnFrameEnd(int ts) {};
		virtual void OnOverlay() {};
	};
}