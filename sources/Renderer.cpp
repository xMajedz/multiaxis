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


void Renderer::Render()
{
    SetWindowTitle(TextFormat("MultiAxis %dFPS", GetFPS()));

    BeginDrawing();
        ClearBackground(bg_color);
	DrawTextureRec(bg.texture, {0, 0, screenWidth, -screenHeight}, {0, 0}, WHITE);
	DrawTextureRec(fg.texture, {0, 0, screenWidth, -screenHeight}, {0, 0}, WHITE);	
	RenderBackground();
 	RenderForeground();
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
