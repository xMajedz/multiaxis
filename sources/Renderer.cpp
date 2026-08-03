#include "Renderer.h"

#include <fstream>

static void UpdateCameraCustom(Camera* camera, Vector3 target, Vector3 rotation, float zoom)
{
    static Vector3 offset {0.f, 6.f, 2.f};
    
    offset = Vector3Scale(Vector3Transform(offset, MatrixRotateXYZ(rotation)), 1.0 + zoom);

    camera->target = target;
	
    camera->position = Vector3Add(target, offset);
}

RenderWindow::RenderWindow(float width, float height, const char* title): screenWidth(width), screenHeight(height)
{
    GetSettings();
    
    SetTraceLogLevel(LOG_ERROR);

    InitWindow(screenWidth, screenHeight, title);
}

RenderWindow::~RenderWindow()
{
    CloseWindow();
}

void RenderWindow::GetSettings()
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

RenderPass::RenderPass()
{
    shaders[BASE_SHADER] = LoadShader("resources/shader/base.vs", "resources/shader/base.fs");
}

RenderPass::~RenderPass()
{
    for (auto shader : shaders)
        UnloadShader(shader);
}

void RenderPass::Draw_(int shapeType, Vector3 size, Color color)
{
    Vector4 normalizedColor = ColorNormalize(color);
    Vector3 objectColor = { normalizedColor.x, normalizedColor.y, normalizedColor.z };
    SetShaderValue(shaders[BASE_SHADER], GetShaderLocation(shaders[BASE_SHADER], "objectColor"), &objectColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(shaders[BASE_SHADER], GetShaderLocation(shaders[BASE_SHADER], "objectAlpha"), &normalizedColor.w, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(shaders[BASE_SHADER]);
    BeginMode3D(camera);
    switch(shapeType)
    {
    case 0:
        DrawCube(Vector3{0}, size.x, size.y, size.z, WHITE);
	break;
    case 1:
        DrawSphere(Vector3{0}, size.x, WHITE);
	break;
    }
    EndMode3D();
    EndShaderMode();
}

void RenderPass::Draw(int shape, Quaternion q, Vector3 p, Vector3 sides, Color color)
{
    float angle;
    Vector3 axis;

    QuaternionToAxisAngle(q, &axis, &angle);
	
    Vector4 normalizedColor = ColorNormalize(color);
    Vector3 objectColor = { normalizedColor.x, normalizedColor.y, normalizedColor.z };
    SetShaderValue(shaders[BASE_SHADER], GetShaderLocation(shaders[BASE_SHADER], "objectColor"), &objectColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(shaders[BASE_SHADER], GetShaderLocation(shaders[BASE_SHADER], "objectAlpha"), &normalizedColor.w, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(shaders[BASE_SHADER]);
    BeginMode3D(camera);
	
    rlPushMatrix();
    rlTranslatef(p.x, p.y, p.z);
    rlRotatef(RAD2DEG * angle, axis.x, axis.y, axis.z);
	
    switch(shape)
    {
    case 0:
        DrawCube(Vector3{0}, sides.x, sides.y, sides.z, color);
	break;
    case 1:
        DrawSphere(Vector3{0}, sides.x, color);
	break;
    }
	
    rlPopMatrix();

    EndMode3D();
    EndShaderMode();
}

Renderer::Renderer(Api& ApiInstance, Game& GameInstance)
  : ApiInstance_(ApiInstance)
  , GameInstance_(GameInstance)
  , window(800, 450, "MultiAxis")
  , bgColor(BLACK) 
{
    ApiInstance_.SetRenderer(this);
    
    renderTextures[RENDER_TEXTURE_BG] = LoadRenderTexture(window.screenWidth, window.screenHeight);
    renderTextures[RENDER_TEXTURE_FG] = LoadRenderTexture(window.screenWidth, window.screenHeight);

    pass.camera.up = {0.f, 0.f, 1.f};
    pass.camera.fovy = 45.f;
    pass.camera.projection = CAMERA_PERSPECTIVE;

    UpdateCameraCustom(&pass.camera, Vector3{0}, Vector3{0}, 0);

    //
    //model = LoadModelFromMesh(GenMeshPlane(340/45, 90/45, 340, 90));
    //model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("scripts/von/von.png");
    //
}

Renderer::~Renderer()
{
    for (auto renderTexture : renderTextures)
        UnloadRenderTexture(renderTexture);
}

#include <cstring>
#include <cstdio>
static void DrawTextStyled(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color color)
{
    if (font.texture.id == 0) font = GetFontDefault();

    int textLen = TextLength(text);

    Color col = color;

    float textOffsetY = 0.0f;
    float textOffsetX = 0.0f;
    float textLineSpacing = 0.0f;
    float scaleFactor = fontSize/font.baseSize;

    for (int i = 0; i < textLen;) {
        int codepointByteCount = 0;
        int codepoint = GetCodepointNext(&text[i], &codepointByteCount);
        
	if (codepoint == '%') {
            i += 1;	    
	    if (text[i++] == '^') {
		char hexbyte[3] {text[i++], text[i++]};
	        uint8_t byte = strtol(hexbyte, NULL, 16);

	        uint8_t r = (byte & 0xE0) >> 5;
                uint8_t g = (byte & 0x1C) >> 2;
      	        uint8_t b = (byte & 0x03);
	    
	        col.r = (r * 255) / 7;
	        col.g = (g * 255) / 7;
	        col.b = (b * 255) / 3;

	        continue;
	    }
	}

        int index = GetGlyphIndex(font, codepoint);
        float increaseX = 0.0f;

        if (font.glyphs[index].advanceX == 0)
	    increaseX = ((float)font.recs[index].width * scaleFactor + spacing);
        else
	    increaseX += ((float)font.glyphs[index].advanceX * scaleFactor + spacing);

	if ((codepoint != ' ') && (codepoint != '\t'))
            DrawTextCodepoint(font, codepoint, (Vector2){ position.x + textOffsetX, position.y + textOffsetY }, fontSize, col);

        textOffsetX += increaseX;
        
        i += codepointByteCount;
    }
}

void Renderer::RenderGame()
{
    ApiInstance_.RenderGame(&pass);
    
    DrawTextStyled(GetFontDefault(), "Text: %^0FText, %^F2Text, %^22Text,", Vector2{200, 200}, 20, 1, WHITE);

    auto frame = GameInstance_.GetFrameData();
    /*
    Vector3 ground_sides = {
        2 * frame.ground_transform.sides.x,
        2 * frame.ground_transform.sides.y,
        2 * frame.ground_transform.sides.z,
    };
    
    pass.Draw(0, QuaternionIdentity(), Vector3{0}, ground_sides, WHITE);
    */
    for (const auto& o : frame.transforms) {    
        Quaternion q {
            o.rotation.v.x,
            o.rotation.v.y,
            o.rotation.v.z,
            o.rotation.s,
        };
    
        Vector3 v {
            o.position.x,
            o.position.y,
            o.position.z,
        };

        Vector3 sides {
            2 * o.sides.x,
            2 * o.sides.y,
            2 * o.sides.z,
        };

	Color color {
	    (uint8_t)(255 * o.color[0]),
	    (uint8_t)(255 * o.color[1]),
	    (uint8_t)(255 * o.color[2]),
	    (uint8_t)(255 * o.color[3]),
	};
    
        pass.Draw(0, q, v, sides, color);
    }
    
    //BeginMode3D(camera);
    //DrawModel(model, (Vector3){0}, 1.f, WHITE);
    //EndMode3D();
}

void Renderer::RenderBackground()
{
    BeginTextureMode(renderTextures[RENDER_TEXTURE_BG]);
        ClearBackground(bgColor);
	ApiInstance_.RenderBackground();
        RenderGame();
    EndTextureMode();
}

void Renderer::RenderForeground()
{
    BeginTextureMode(renderTextures[RENDER_TEXTURE_FG]);
	ClearBackground(BLANK);
	ApiInstance_.RenderForeground();
    EndTextureMode();
}

static void ProcessMouseInput(Api* ApiInstance)
{
    static Vector2 previousPosition {0};
 
    Vector2 position = GetMousePosition();

    if (previousPosition.x != position.x || previousPosition.y != position.y) {
        ApiInstance->MouseMoved(position.x, position.y);
	previousPosition = position;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        ApiInstance->MouseButtonPressed(MOUSE_BUTTON_LEFT, position.x, position.y);
    
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        ApiInstance->MouseButtonReleased(MOUSE_BUTTON_LEFT, position.x, position.y);
}

static void ProcessKeyboardInput(Api* ApiInstance)
{
    static int previousKey = 0;

    int key = GetKeyPressed();

    if (IsKeyPressed(key)) {
        ApiInstance->KeyPressed(key);
	previousKey = key;
    }

    if (IsKeyReleased(previousKey)) {
        ApiInstance->KeyReleased(previousKey);
	previousKey = key;	
    }
}

static RayCollision ProcessRayCollision(Ray ray, Vector3 position, Vector3 sides)
{
    RayCollision col {0};

    Vector3 min {
        position.x - sides.x,
        position.y - sides.y,
        position.z - sides.z,
    };
    
    Vector3 max {
        position.x + sides.x,
        position.y + sides.y,
        position.z + sides.z,
    };

    col = GetRayCollisionBox(ray, BoundingBox{min, max});

    return col;
}

static bool ProcessMouseRay(Camera camera, Vector3 position, Vector3 sides)
{
    Ray ray = GetMouseRay(GetMousePosition(), camera);

    RayCollision col1 {0};
    RayCollision col2 {0};

    //for () {
    col1 = ProcessRayCollision(ray, position, sides);

    if (col1.hit && (col2.distance == 0 || col2.distance > col1.distance))
        col2 = col1;
    //}
    return col1.hit;
}

void Renderer::Render()
{
    /* Input Handling */
    
    ProcessMouseInput(&ApiInstance_);

    ProcessKeyboardInput(&ApiInstance_);
    
    /* Camera Controls */

    auto frame = GameInstance_.GetFrameData();

    Vector3 body_position {
        frame.body_transform.position.x,
        frame.body_transform.position.y,
        frame.body_transform.position.z,
    };
    
    Vector3 body_sides {
        frame.body_transform.sides.x,
        frame.body_transform.sides.y,
        frame.body_transform.sides.z,
    };
        
    Vector3 target = body_position;
    
    if (IsKeyDown(KEY_LEFT_SHIFT)) {
        if (IsKeyDown(KEY_W))
	    UpdateCameraCustom(&pass.camera, target, Vector3{-1 * DEG2RAD, 0.f, 0.f}, 0);

        if (IsKeyDown(KEY_A))
            UpdateCameraCustom(&pass.camera, target, Vector3{0.f, 0.f,  1 * DEG2RAD}, 0);      

        if (IsKeyDown(KEY_S))
	    UpdateCameraCustom(&pass.camera, target, Vector3{1 * DEG2RAD, 0.f, 0.f}, 0);

        if (IsKeyDown(KEY_D))
            UpdateCameraCustom(&pass.camera, target, Vector3{0.f, 0.f, -1 * DEG2RAD}, 0);
    } else {
        if (IsKeyDown(KEY_W))
            UpdateCameraCustom(&pass.camera, target, Vector3{0}, -0.01);

        if (IsKeyDown(KEY_A))
            UpdateCameraCustom(&pass.camera, target, Vector3{0.f, 0.f, 5 * DEG2RAD}, 0);      

        if (IsKeyDown(KEY_S))
            UpdateCameraCustom(&pass.camera, target, Vector3{0}, 0.01);

        if (IsKeyDown(KEY_D))
            UpdateCameraCustom(&pass.camera, target, Vector3{0.f, 0.f, -5 * DEG2RAD}, 0);
    }

    UpdateCameraCustom(&pass.camera, target, Vector3{0}, 0);

    /* UpdateCameraPro is good for zoom and free cam movement */

    if (ProcessMouseRay(pass.camera, body_position, body_sides))
        ApiInstance_.Log("1");
    
    SetWindowTitle(TextFormat("MultiAxis %dFPS", GetFPS()));

    RenderBackground();
    RenderForeground();
    
    BeginDrawing();
        DrawTextureRec(renderTextures[RENDER_TEXTURE_BG].texture, {0, 0, window.screenWidth, -window.screenHeight}, {0, 0}, WHITE);
	DrawTextureRec(renderTextures[RENDER_TEXTURE_FG].texture, {0, 0, window.screenWidth, -window.screenHeight}, {0, 0}, WHITE);	
    EndDrawing();
}
