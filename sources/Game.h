#pragma once

#include "Api.h"

#include "box3d/box3d.h"

#include <unordered_map>

#include <vector>
#include <array>

#ifndef GAME_VERSION
#define GAME_VERSION "git-box3d"
#endif

struct env_obj {
    b3Vec3 sides;
    b3Vec3 position;
    b3Quat rotation;

    int shape;
  
    uint32_t flag;

    std::array<float, 4> color;
};

struct env_obj_joint {
    b3Vec3 sides;
    b3Vec3 position;
    b3Quat rotation;
};

struct GameRules {
    b3Vec3 gravity;
  
    float engagedistance;
    float engageheight;

    int turnframes;
    int numplayers;	
};

struct GameMod {
    GameRules rules;
  
    std::unordered_map<std::string, uint32_t> o_map;

    std::vector<env_obj> objects;
    std::vector<env_obj_joint> joints;
};

struct FrameTransform {
    b3BodyId id;

    b3Vec3 sides;
    b3Vec3 position;
    b3Quat rotation;

    uint32_t flag;

    std::array<float, 4> color;
};

struct FrameData {
    std::vector<FrameTransform> transforms;

    FrameTransform ground_transform;
    FrameTransform body_transform;
};

struct GamePhysics {
    GamePhysics(Api& ApiInstance, FrameData& frame, GameMod* mod);
    ~GamePhysics();

    void NewWorld();

    void Step(FrameData& frame);

    Api& ApiInstance_;

    FrameData& frame_;

    GameMod* mod_;

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
