#include "Game.h"

void Game::Update()
{
    ApiInstance_.Update();
}

Game::Game(Api& ApiInstance)
  : ApiInstance_(ApiInstance)
  , running_(true)
{
    ApiInstance_.SetGame(this);
}

Game::~Game()
{
}

void Game::Quit()
{
    running_ = false;
}

bool Game::ShouldQuit()
{
    return !running_;
}
