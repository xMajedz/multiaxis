#include "Game.h"

Game::Game(Api& ApiInstance)
  : ApiInstance_(ApiInstance)
  , physics_(nullptr)
  , running_(true)
{
    ApiInstance_.SetGame(this);
}

Game::~Game()
{
    delete physics_;
}

void Game::Update()
{
    ApiInstance_.Update();

    
    if (physics_) physics_->Step(frame_);
}

FrameData Game::GetFrameData()
{
    return frame_;
}

void Game::Quit()
{
    running_ = false;
}

void Game::NewGame()
{
    delete physics_;
    physics_ = new GamePhysics(ApiInstance_);
}

bool Game::ShouldQuit()
{
    return !running_;
}

GamePhysics::GamePhysics(Api& ApiInstance) : ApiInstance_(ApiInstance)
{
    b3WorldDef worldDef = b3DefaultWorldDef();
    worldDef.gravity = {0.0f, 0.0f, -1.0f};

    worldId = b3CreateWorld(&worldDef);

    /* static */
    b3BodyDef groundDef = b3DefaultBodyDef();
    groundDef.position = {0.0f, 0.0f, -1.0f};

    b3BodyId groundId = b3CreateBody(worldId, &groundDef);

    ground_sides = {2.0f, 2.0f, 1.0f};
    b3BoxHull groundBox = b3MakeBoxHull(ground_sides.x, ground_sides.y, ground_sides.z);

    b3ShapeDef groundShapeDef = b3DefaultShapeDef();
    b3CreateHullShape(groundId, &groundShapeDef, &groundBox.base);

    /* dynamic */
    b3BodyDef bodyDef = b3DefaultBodyDef();
    bodyDef.type = b3_dynamicBody;
    bodyDef.position = {0.0f, 0.0f, 5.0f};

    bodyId = b3CreateBody(worldId, &bodyDef);

    body_sides = {1.0f, 1.0f, 1.0f};
    b3BoxHull dynamicBox = b3MakeBoxHull(body_sides.x, body_sides.y, body_sides.z);

    b3ShapeDef shapeDef = b3DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.baseMaterial.friction = 0.3f;
    
    b3CreateHullShape(bodyId, &shapeDef, &dynamicBox.base);
}

GamePhysics::~GamePhysics()
{
    b3DestroyWorld(worldId);
}

void GamePhysics::NewWorld()
{
}

void GamePhysics::Step(FrameData& frame)
{
    b3World_Step(worldId, (1.0f / 60.0f), 4);

    frame.ground_transform.sides = ground_sides;

    frame.body_transform.sides = body_sides;    

    frame.body_transform.position = b3Body_GetPosition(bodyId);
    frame.body_transform.rotation = b3Body_GetRotation(bodyId);    
}
