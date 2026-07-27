#pragma once

#include "Game.h"

#include "raylib.h"

#include "raymath.h"

enum RenderTextureType {
    RENDER_TEXTURE_BG,
    RENDER_TEXTURE_FG,
    
    RENDER_TEXTURE_COUNT,
};

enum ShaderType {
    BASE_SHADER,
  
    SHADER_COUNT,
};

class Renderer {
public:
    Renderer(Api& ApiInstance, Game& GameInstance);
    ~Renderer();

    void Render();
  
    void Draw(int shapeType, Vector3 size, Color color);

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
	
    Color bgColor;

    RenderTexture renderTextures[RENDER_TEXTURE_COUNT];

    Camera camera;

    Shader shaders[SHADER_COUNT];  
};
