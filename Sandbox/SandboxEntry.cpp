#include "Core/Main.h"

#include "SandboxGame.h"


extern std::unique_ptr<Hyperion::Game> createGame()
{
	return std::make_unique<SandboxGame>();
}