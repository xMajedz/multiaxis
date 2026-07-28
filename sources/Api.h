#pragma once

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include <cstdlib>

#include <string>

class Game;
class Renderer;
class RenderPass;

class Api {
public:
    Api();
    ~Api();

    void Boot(const std::string& bootfile);
    void Log(const std::string& message);
  
    void SetGame(Game* GameInstance);
    void SetRenderer(Renderer* RendererInstance);
  
    void Update();
    void Console(const std::string& message);

    void RenderGame(RenderPass* renderPass);

    void RenderBackground();
    void RenderForeground();

    void MouseMoved(float x, float y);
    void MouseButtonPressed(int btn, float x, float y);
    void MouseButtonReleased(int btn, float x, float y);

    void KeyPressed(int key);
    void KeyReleased(int key);

    int require_builtin(lua_State* L, const std::string& filename);
  
private:
    lua_State* ML;
};

int Api_AddHook(lua_State* L);

enum HookType {
    PROTOTYPE = 0,

    CONSOLE,
    UPDATE,

    RENDER_GAME,

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

    {0, "OnRenderGame"},

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

int luaL_registerwithclosure(lua_State* L, const char* libname, const luaL_Reg* l, int upvalues);

int luaopenApiRaylib(lua_State* L);
int luaopenApiRaygui(lua_State* L);
int luaopenApiRaymath(lua_State* L);

int luaopen_uielement(lua_State* L);
int luaopen_uielement3d(lua_State* L);

static const struct { std::string name; int (*func)(lua_State*); } builtinlibs[] {
    {"@raylib",      luaopenApiRaylib},
    {"@raygui",      luaopenApiRaygui},
    {"@raymath",     luaopenApiRaymath},

    {"@uielement",   luaopen_uielement},
    {"@uielement3d", luaopen_uielement3d},
};

int luaopenApi(lua_State* L);
int luaopenGame(lua_State* L);
int luaopenRenderer(lua_State* L);
