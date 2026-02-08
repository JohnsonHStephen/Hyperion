#pragma once

namespace Hyperion
{
	class FrameListener {
	public:
		FrameListener() {};
		virtual ~FrameListener() {};

		virtual void onAttach() {};
		virtual void onDetach() {};

		virtual void onFrameStart(int ts) {};
		virtual void onFrameEnd(int ts) {};
		virtual void onOverlay() {};
	};
}