// Hyperion.cpp : Defines the entry point for the application.
//

#include "Application.h"
#include "Hyperion.h"

extern Hyperion::Application* Hyperion::createApplication();

int main() {
	Hyperion::Application* app = Hyperion::createApplication();

	app->run();

	exit(EXIT_SUCCESS);
}