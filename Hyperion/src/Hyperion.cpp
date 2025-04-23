// Hyperion.cpp : Defines the entry point for the application.
//

#include "Hyperion.h"

#include "Application.h"

int main() {
	Hyperion::Application* app = Hyperion::Application::Create();

	app->Run();

	exit(EXIT_SUCCESS);
}