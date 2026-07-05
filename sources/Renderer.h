#include "raylib.h"
#include "Game.h"

class Renderer {
public:
    static Renderer& GetInstance()
    {
       static Renderer RendererInstance;
       return RendererInstance;
    }
	
    void Render();
    void RenderGame();
    void RenderBackground();
    void RenderForeground();

    void SetApiInstance(Api* ApiInstance);
    void SetGameInstance(Game* GameInstance);

    void GetSettings();
private:
    float screenWidth;
    float screenHeight;
	
    Color bg_color;

    RenderTexture bg;
    RenderTexture fg;

    Camera3D camera;
  
    Api* ApiInstance_ = nullptr;	
    Game* GameInstance_ = nullptr;

    Renderer();
    ~Renderer();
};
