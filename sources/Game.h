#pragma once

#include "Api.h"

#define GAME_VERSION "git-box3d"

class Game {
public:
    Game(Api& ApiInstance);
    ~Game();
  
    void Quit();
    bool ShouldQuit();
  
    void Update();

    void SetApiInstance(Api* ApiInstance);
private:
    Api& ApiInstance_;

    bool running_ = false;
};
