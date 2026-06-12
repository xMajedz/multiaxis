#include "api.h"
#include "game.h"

#include <cstring>

int main(int argc, char* argv[])
{
    const char* bootfile = "boot";

	for (int i = 0; i < argc; i += 1) {
	    const char* arg = argv[i];
	    if (strcmp(arg, "--bootfile") == 0) {
            i += 1;
		    bootfile = argv[i];
	    }
	}

	Window::Init();

    Game::Init();

	Api::Init();
	
    Api::Boot(bootfile);

    bool running = true;

    while (running)
    {
        Window::Update();

        Game::Update(Game::GetFrameTime());
        
        Window::Draw();

        running = Game::Running() && !raylib::WindowShouldClose();

        Console::Update();
    }

    Game::Quit();

    Window::Close();
}
