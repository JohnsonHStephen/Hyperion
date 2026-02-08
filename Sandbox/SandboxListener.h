#pragma once

#include "Core/Hyperion.h"

class SandboxListener: public Hyperion::FrameListener
{
public:
	SandboxListener();
	~SandboxListener();

	void onAttach() override;
	void onDetach() override;

	void onFrameStart(int ts) override;
	void onFrameEnd(int ts) override;
	void onOverlay() override;
};
