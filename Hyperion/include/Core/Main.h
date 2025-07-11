// Hyperion.cpp : Defines the entry point for the application.
//

#include "Application.h"
#include "Hyperion.h"

extern Hyperion::Application* Hyperion::CreateApplication();

int main() {
	Hyperion::Application* app = Hyperion::CreateApplication();

	app->Run();

	exit(EXIT_SUCCESS);
}