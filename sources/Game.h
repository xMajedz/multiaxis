#pragma once

#include "Api.h"

#define GAME_VERSION "git-box3d"

class Game {
public:	
    static Game& GetInstance()
    {
        static Game GameInstance;
        return GameInstance;
    }
    
    void Update();

    void SetApiInstance(Api* ApiInstance);
private:
    Api* ApiInstance_ = nullptr;

    Game();
    ~Game();
};
