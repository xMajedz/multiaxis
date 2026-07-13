#include "Api.h"
#include "Game.h"
#include "Renderer.h"

#include <iostream>

#include <cstring>

void help(void)
{
    const char* help_message =
	  "Usage: multiaxis <flag>\n"
	  "\tmultiaxis --help -h\n"
	  "\tmultiaxis --version -v\n"
	  "\tmultiaxis --bootfile -b <file>"
	;
    std::cout << help_message << std::endl;
}

int main(int argc, char* argv[])
{
    const char* bootfile = "boot.luau";

    for (int i = 0; i < argc; i += 1) {
	const char* arg = argv[i];

	if ((strcmp(arg, "--bootfile") == 0) || strcmp(arg, "-b") == 0) {
            i += 1;
	    bootfile = argv[i];
	} else if ((strcmp(arg, "--help") == 0) || (strcmp(arg, "-h") == 0)) {
            i += 1;
	    help();
	    return 0;
	} else if ((strcmp(arg, "--version") == 0) || (strcmp(arg, "-v") == 0)) {
            i += 1;
	    std::cout << GAME_VERSION << std::endl;
	    return 0;
        }
    }

    Api& ApiInstance = Api::GetInstance();

    Game& GameInstance = Game::GetInstance();
    GameInstance.SetApiInstance(&ApiInstance);
	
    Renderer& RendererInstance = Renderer::GetInstance();
    RendererInstance.SetApiInstance(&ApiInstance);
    RendererInstance.SetGameInstance(&GameInstance);

    ApiInstance.Boot(bootfile);

    bool running = true;
    while (running) {
	GameInstance.Update();
	RendererInstance.Render();
		
	running = !WindowShouldClose();
    }
}
