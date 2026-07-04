#include "Game.h"

void Game::Draw()
{
    BeginDrawing();
        ClearBackground(BLACK);
	EndDrawing();
}

Game::Game()
{
    InitWindow(800, 450, "MultiAxis");
}

Game::~Game()
{
    CloseWindow();
}
