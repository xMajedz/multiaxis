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
private:
    lua_State* ML;

    Api();
    ~Api();
};

enum Hook {
    PROTOTYPE = 0,

    CONSOLE,
    UPDATE,

    RENDER_BG,
    RENDER_FG,
    
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

static Hook HookList[HOOK_COUNT];

static const char* Hooks[HOOK_COUNT] = {
   "prototype",

   "Console",
   "Update",

   "RenderBackground",
   "RenderForeground",

    /*	"NewGame",
	"Freeze",
	"Step",
	"Draw",
	"Draw3D",
	"NearCallback",
	"FileDropped",*/
};

