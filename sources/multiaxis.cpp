#include "api.h"
#include "game.h"

int main(int argc, char* argv[])
{
    Window::Init();

    Game::Init();

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
