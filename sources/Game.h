#pragma once

#include "Api.h"

#include "box3d/box3d.h"

#ifndef GAME_VERSION
#define GAME_VERSION "git-box3d"
#endif

struct FrameTransform {
    b3Vec3 sides;
    b3Vec3 position;
    b3Quat rotation;
};

struct FrameData {
    FrameTransform ground_transform;
    FrameTransform body_transform;
};

struct GamePhysics {
    GamePhysics(Api& ApiInstance);
    ~GamePhysics();

    void NewWorld();

    void Step(FrameData& frame);

    Api& ApiInstance_;

    b3WorldId worldId;

    b3BodyId bodyId;

    b3Vec3 body_sides;
    b3Vec3 ground_sides;
};

class Game {
public:
    Game(Api& ApiInstance);
    ~Game();
  
    void Quit();

    void NewGame();

    bool ShouldQuit();
  
    void Update();
  
    FrameData GetFrameData();
    FrameData GetFreezeData();
private:
    Api& ApiInstance_;

    GamePhysics* physics_;

    FrameData frame_;
    FrameData freeze_;

    bool running_ = false;
};
