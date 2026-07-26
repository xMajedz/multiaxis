#pragma once

#include "Game.h"

#include "raylib.h"

#include "raymath.h"

enum ShaderType {
    BASE_SHADER,
  
    SHADER_COUNT,
};

class Renderer {
public:
    Renderer(Api& ApiInstance, Game& GameInstance);
    ~Renderer();

    void Render();
    void RenderGame();
    void RenderBackground();
    void RenderForeground();

    void SetApiInstance(Api* ApiInstance);
    void SetGameInstance(Game* GameInstance);

    void GetSettings();
private:
    Api& ApiInstance_;	
    Game& GameInstance_;

    float screenWidth;
    float screenHeight;
	
    Color bg_color;

    RenderTexture bg;
    RenderTexture fg;

    Camera camera;

    Shader shaders[SHADER_COUNT];  
};
