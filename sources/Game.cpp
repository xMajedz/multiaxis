#include "Game.h"

void Game::Update()
{
    if (ApiInstance_ != nullptr) ApiInstance_->Update();
}

Game::Game()
{
}

Game::~Game()
{
}

void Game::SetApiInstance(Api* ApiInstance)
{
    ApiInstance_ = ApiInstance;
}
