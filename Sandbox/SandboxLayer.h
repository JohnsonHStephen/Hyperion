#pragma once

#include "Core/Hyperion.h"

namespace Hyperion
{
	class SandboxLayer: public Layer {
		public:
			SandboxLayer();
			~SandboxLayer();

			void onAttach();
			void OnDetach();

			void OnFrameStart(int ts);
			void OnFrameEnd(int ts);
			void OnOverlay();
		};
}
