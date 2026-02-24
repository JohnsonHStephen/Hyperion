// Hyperion.cpp : Defines the entry point for the application.
//

#include "Application.h"
#include "Game.h"

extern std::unique_ptr<Hyperion::Game> createGame();

int main() {
	std::unique_ptr<Hyperion::Game> game = createGame();

	Hyperion::Application app = Hyperion::Application(game->getAppInfo());

	app.run(game.get());

	exit(EXIT_SUCCESS);
}