#pragma once

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include <cstdlib>

#include <string>

class Api {
public:
    static Api& GetInstance()
    {
        static Api ApiInstance;
        return ApiInstance;
    }
  
    void Boot(const std::string& bootfile);

    void Update();
    void Console(const std::string& message);

    void RenderBackground();
    void RenderForeground();

    void MouseMoved(float x, float y);
    void MouseButtonPressed(int btn, float x, float y);
    void MouseButtonReleased(int btn, float x, float y);

    void KeyPressed(int key);
    void KeyReleased(int key);
  
private:
    lua_State* ML;

    Api();
    ~Api();
};

enum HookType {
    PROTOTYPE = 0,

    CONSOLE,
    UPDATE,

    RENDER_BG,
    RENDER_FG,

    MOUSE_MOVED,
    MOUSE_PRESSED,
    MOUSE_RELEASED,

    KEY_PRESSED,
    KEY_RELEASED,

  /*
  NEW_GAME,
  FREEZE,
  STEP,
  DRAW,
  DRAW3D,
  FILE_DROPPED,
  */
    HOOK_COUNT,
};

static struct { int key; const char * name; } Hooks[HOOK_COUNT] {
    {0, "prototype"},

    {0, "Console"},
    {0, "Update"},

    {0, "OnRenderBackground"},
    {0, "OnRenderForeground"},

    {0, "OnMouseMoved"},
    {0, "OnMouseButtonPressed"},
    {0, "OnMouseButtonReleased"},

    {0, "OnKeyPressed"},
    {0, "OnKeyReleased"},

    /*	"NewGame",
	"Freeze",
	"Step",
	"Draw",
	"Draw3D",
	"NearCallback",
	"FileDropped",*/
};

