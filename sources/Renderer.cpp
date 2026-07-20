#include "Renderer.h"

#include <fstream>

Renderer::Renderer() : screenWidth(800), screenHeight(450), bg_color(BLACK) 
{
    GetSettings();
    
    SetTraceLogLevel(LOG_ERROR);

    InitWindow(screenWidth, screenHeight, "MultiAxis");

    bg = LoadRenderTexture(screenWidth, screenHeight);
    fg = LoadRenderTexture(screenWidth, screenHeight);
}

Renderer::~Renderer()
{
    UnloadRenderTexture(bg);
    UnloadRenderTexture(fg);

    CloseWindow();
}

void Renderer::GetSettings()
{
    std::ifstream file("settings.txt");
	
    if (!file) return ;

    std::string line;

    while (std::getline(file, line)) {
	 size_t s = line.find('=');
	 std::string setting = line.substr(0, s);
	 std::string value   = line.substr(s + 1);
	 if (setting == "screenWidth") {
	     screenWidth  = std::stoi(value);
	 } else if (setting == "screenHeight") {
	     screenHeight = std::stoi(value);
	 } else if (setting == "fullScreen") {
	   //fullscreen_mode = (bool)std::stoi(value);
	 }
    }
}

void Renderer::RenderGame()
{
    if (GameInstance_ == nullptr) return;
}

void Renderer::RenderBackground()
{
    BeginTextureMode(bg);
        ClearBackground(bg_color);
	ApiInstance_->RenderBackground();
        RenderGame();
    EndTextureMode();
}

void Renderer::RenderForeground()
{
    BeginTextureMode(fg);
	ClearBackground(BLANK);
	ApiInstance_->RenderForeground();
    EndTextureMode();
}

Vector2 previousMousePosition = {0};
int previousKey = 0;

void Renderer::Render()
{
    /* Input Handling*/
    Vector2 mousePosition = GetMousePosition();
    if (previousMousePosition.x != mousePosition.x || previousMousePosition.y != mousePosition.y) {
        ApiInstance_->MouseMoved(mousePosition.x, mousePosition.y);
	previousMousePosition = mousePosition;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        ApiInstance_->MouseButtonPressed(MOUSE_BUTTON_LEFT, mousePosition.x, mousePosition.y);
    }
    
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        ApiInstance_->MouseButtonReleased(MOUSE_BUTTON_LEFT, mousePosition.x, mousePosition.y);
    }

    int key = GetKeyPressed();
    if (IsKeyPressed(key)) {
        ApiInstance_->KeyPressed(key);
	previousKey = key;
    }

    if (IsKeyReleased(previousKey)) {
        ApiInstance_->KeyReleased(previousKey);
	previousKey = key;	
    }

    
    SetWindowTitle(TextFormat("MultiAxis %dFPS", GetFPS()));

    RenderBackground();
    RenderForeground();
    
    BeginDrawing();
    //ClearBackground(bg_color);
	DrawTextureRec(bg.texture, {0, 0, screenWidth, -screenHeight}, {0, 0}, WHITE);
	DrawTextureRec(fg.texture, {0, 0, screenWidth, -screenHeight}, {0, 0}, WHITE);	
    EndDrawing();
}

void Renderer::SetApiInstance(Api* ApiInstance)
{
    ApiInstance_ = ApiInstance;
}

void Renderer::SetGameInstance(Game* GameInstance)
{
    GameInstance_ = GameInstance;
}
