#include "Renderer.h"

#include <fstream>

static Model model;

static void UpdateCameraCustom(Camera* camera, Vector3 target, Vector3 rotation, float zoom)
{
    static Vector3 offset {0.f, 6.f, 2.f};
    
    offset = Vector3Scale(Vector3Transform(offset, MatrixRotateXYZ(rotation)), 1.0 + zoom);

    camera->target = target;
	
    camera->position = Vector3Add(target, offset);
}

Renderer::Renderer(Api& ApiInstance, Game& GameInstance)
  : ApiInstance_(ApiInstance)
  , GameInstance_(GameInstance)
  , screenWidth(800)
  , screenHeight(450)
  , bg_color(BLACK) 
{
    ApiInstance_.SetRenderer(this);
    
    GetSettings();
    
    SetTraceLogLevel(LOG_ERROR);

    InitWindow(screenWidth, screenHeight, "MultiAxis");

    bg = LoadRenderTexture(screenWidth, screenHeight);
    fg = LoadRenderTexture(screenWidth, screenHeight);

    camera.up = {0.f, 0.f, 1.f};
    camera.fovy = 45.f;
    camera.projection = CAMERA_PERSPECTIVE;

    UpdateCameraCustom(&camera, Vector3{0}, Vector3{0}, 0);

    shaders[BASE_SHADER] = LoadShader("resources/shader/base.vs", "resources/shader/base.fs");

    //
    //model = LoadModelFromMesh(GenMeshPlane(340/45, 90/45, 340, 90));
    //model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("scripts/von/von.png");
    //
}

Renderer::~Renderer()
{
    UnloadRenderTexture(bg);
    UnloadRenderTexture(fg);

    UnloadShader(shaders[BASE_SHADER]);

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
    Color color = GREEN;
    
    Vector4 normalizedColor = ColorNormalize(color);
    Vector3 objectColor = { normalizedColor.x, normalizedColor.y, normalizedColor.z };
    SetShaderValue(shaders[BASE_SHADER], GetShaderLocation(shaders[BASE_SHADER], "objectColor"), &objectColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(shaders[BASE_SHADER], GetShaderLocation(shaders[BASE_SHADER], "objectAlpha"), &normalizedColor.w, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(shaders[BASE_SHADER]);
    BeginMode3D(camera);
    
    DrawCube(Vector3{0}, 0.5, 0.5, 0.5, WHITE);
    //DrawSphere(Vector3{0}, 0.25, WHITE);

    EndMode3D();
    EndShaderMode();

    //BeginMode3D(camera);
    //DrawModel(model, (Vector3){0}, 1.f, WHITE);
    //EndMode3D();
}

void Renderer::RenderBackground()
{
    BeginTextureMode(bg);
        ClearBackground(bg_color);
	ApiInstance_.RenderBackground();
        RenderGame();
    EndTextureMode();
}

void Renderer::RenderForeground()
{
    BeginTextureMode(fg);
	ClearBackground(BLANK);
	ApiInstance_.RenderForeground();
    EndTextureMode();
}

Vector2 previousMousePosition = {0};
int previousKey = 0;

void Renderer::Render()
{
    /* Input Handling */
    Vector2 mousePosition = GetMousePosition();
    if (previousMousePosition.x != mousePosition.x || previousMousePosition.y != mousePosition.y) {
        ApiInstance_.MouseMoved(mousePosition.x, mousePosition.y);
	previousMousePosition = mousePosition;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        ApiInstance_.MouseButtonPressed(MOUSE_BUTTON_LEFT, mousePosition.x, mousePosition.y);
    }
    
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        ApiInstance_.MouseButtonReleased(MOUSE_BUTTON_LEFT, mousePosition.x, mousePosition.y);
    }

    int key = GetKeyPressed();
    if (IsKeyPressed(key)) {
        ApiInstance_.KeyPressed(key);
	previousKey = key;
    }

    if (IsKeyReleased(previousKey)) {
        ApiInstance_.KeyReleased(previousKey);
	previousKey = key;	
    }
    
    /* Camera Controls */
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        if (IsKeyDown(KEY_W))
	    UpdateCameraCustom(&camera, Vector3{0}, Vector3{-1 * DEG2RAD, 0.f, 0.f}, 0);

        if (IsKeyDown(KEY_A))
            UpdateCameraCustom(&camera, Vector3{0}, Vector3{0.f, 0.f,  1 * DEG2RAD}, 0);      

        if (IsKeyDown(KEY_S))
	    UpdateCameraCustom(&camera, Vector3{0}, Vector3{1 * DEG2RAD, 0.f, 0.f}, 0);

        if (IsKeyDown(KEY_D))
            UpdateCameraCustom(&camera, Vector3{0}, Vector3{0.f, 0.f, -1 * DEG2RAD}, 0);
    } else {
        if (IsKeyDown(KEY_W))
            UpdateCameraCustom(&camera, Vector3{0}, Vector3{0}, -0.01);

        if (IsKeyDown(KEY_A))
            UpdateCameraCustom(&camera, Vector3{0}, Vector3{0.f, 0.f, 5 * DEG2RAD}, 0);      

        if (IsKeyDown(KEY_S))
            UpdateCameraCustom(&camera, Vector3{0}, Vector3{0}, 0.01);

        if (IsKeyDown(KEY_D))
            UpdateCameraCustom(&camera, Vector3{0}, Vector3{0.f, 0.f, -5 * DEG2RAD}, 0);
    }

    /* UpdateCameraPro is good for zoom and free cam movement */

    SetWindowTitle(TextFormat("MultiAxis %dFPS", GetFPS()));

    RenderBackground();
    RenderForeground();
    
    BeginDrawing();
        DrawTextureRec(bg.texture, {0, 0, screenWidth, -screenHeight}, {0, 0}, WHITE);
	DrawTextureRec(fg.texture, {0, 0, screenWidth, -screenHeight}, {0, 0}, WHITE);	
    EndDrawing();
}
