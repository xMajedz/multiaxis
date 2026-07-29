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

struct RenderWindow {
    RenderWindow(float width, float height, const char* title);
    ~RenderWindow();

    void GetSettings();

    float screenWidth;
    float screenHeight;
};

struct RenderPass {
    RenderPass();
    ~RenderPass();

    void Draw(int shapeType, Vector3 size, Color color);

    Camera camera;
  
    Shader shaders[SHADER_COUNT];    
};

class Renderer {
public:
    Renderer(Api& ApiInstance, Game& GameInstance);
    ~Renderer();

    void Render();

    void RenderGame();
    void RenderBackground();
    void RenderForeground();

private:
    Api& ApiInstance_;	
    Game& GameInstance_;

    Color bgColor;

    RenderWindow window;

    RenderPass pass;

    RenderTexture renderTextures[RENDER_TEXTURE_COUNT];
};
