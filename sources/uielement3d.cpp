#include "Api.h"

#include "Renderer.h"

#include "raylib.h"

enum UI3DShapeType {
  CUBE,
  SPHERE,
};

struct uielement3d
{
    uielement3d* parent;

    uint32_t globalId = 1000;

    Vector3 pos = { 0, 0, 0 };
    Vector3 size = { 1, 1, 1 };
  
    UI3DShapeType shapeType = CUBE;

    Color bgColor = GREEN;

    bool viewport = false;
  
    Api* ApiInstance;

    void display(RenderPass* renderPass);
  
    uielement3d(Api* ApiInst);
    ~uielement3d();
};

#define lua_newuielement3d(L) static_cast<uielement3d*>(lua_newuserdatataggedwithmetatable(L, sizeof(uielement3d), 2))
#define lua_isuielement3d(L, idx) (lua_isuserdata(L, idx) && lua_userdatatag(L, idx) == 2)
#define lua_touielement3d(L, idx) static_cast<uielement3d*>(lua_touserdatatagged(L, idx, 2))

void uielement3d::display(RenderPass* pass)
{
    pass->Draw((int)shapeType, size, bgColor);
}

uielement3d::uielement3d(Api* ApiInst) : ApiInstance(ApiInst)
{
    ApiInstance->Log(TextFormat("uielement3d.new: %p", this));
}

uielement3d::~uielement3d()
{
  //ApiInstance->Log(TextFormat("~uielement3d: %p", this));
}

static int uielement3d_new(lua_State* L)
{
    Api* ApiInstance = static_cast<Api*>(lua_tolightuserdata(L, lua_upvalueindex(1)));

    uielement3d* elem = new (lua_newuielement3d(L)) uielement3d(ApiInstance);

    lua_getfield(L, -2, "pos");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, 3, 1);
        elem->pos.x = lua_tointeger(L, -1);
        lua_rawgeti(L, 3, 2);
        elem->pos.y = lua_tointeger(L, -1);
        lua_rawgeti(L, 3, 3);
        elem->pos.z = lua_tointeger(L, -1);
        lua_pop(L, 3);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -2, "size");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, 3, 1);
        elem->size.x = lua_tointeger(L, -1);
        lua_rawgeti(L, 3, 2);
        elem->size.y = lua_tointeger(L, -1);
        lua_rawgeti(L, 3, 3);
        elem->size.z = lua_tointeger(L, -1);
        lua_pop(L, 3);
    }
    lua_pop(L, 1);

    lua_getfield(L, -2, "bgColor");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, 3, 1);
        elem->bgColor.r = (255 * lua_tonumber(L, -1));
        lua_rawgeti(L, 3, 2);
        elem->bgColor.g = (255 * lua_tonumber(L, -1));
        lua_rawgeti(L, 3, 3);
	elem->bgColor.b = (255 * lua_tonumber(L, -1));
        lua_rawgeti(L, 3, 4);
        elem->bgColor.a = (255 * lua_tonumber(L, -1));
	lua_pop(L, 4);
    }
    lua_pop(L, 1);

    lua_pushstring(L, "UIElement3DManager");
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_pushvalue(L, -2);
    lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
    lua_pop(L, 1);

    if (elem->viewport) {
        lua_pushstring(L, "UIVisual3DManagerViewport");
    } else {
        lua_pushstring(L, "UIVisual3DManager");
    }
    
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_pushunsigned(L, elem->globalId);
    lua_gettable(L, -2);
    if (lua_isnil(L, -1)) {
	lua_pop(L, 1);
	lua_pushunsigned(L, elem->globalId);
	lua_newtable(L);
        lua_settable(L, -3);

	lua_pushunsigned(L, elem->globalId);
        lua_gettable(L, -2);
    }
    
    lua_pushvalue(L, 2);
    lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
    
    lua_pop(L, 2);
    
    return 1;
}

#include <iostream>

static int uielement3d_display(lua_State* L)
{
    RenderPass* pass = static_cast<RenderPass*>(lua_tolightuserdata(L, 2));
    lua_touielement3d(L, 1)->display(pass);
    return 0;
}

static int uielement3d_drawVisuals(lua_State* L)
{
    uint32_t globalId;
    
    if (lua_gettop(L) == 0)
        return 0;

    if (lua_istable(L, 1)) {
        lua_getfield(L, 1, "globalId");
	if (lua_isnumber(L, -1)) {
	    globalId = lua_tounsigned(L, -1);
	    lua_pop(L, 1);
	}
	lua_pop(L, 1);
    }
    
    if (lua_isnumber(L, 1)) {
        globalId = lua_tounsigned(L, 1);
    }
    
    lua_pushstring(L, "UIVisual3DManager");
    lua_gettable(L, LUA_REGISTRYINDEX);
            
    lua_pushunsigned(L, globalId);
    lua_gettable(L, -2);

    if (!lua_istable(L, -1))
        return 0;
	  
    for (int i = 1; lua_objlen(L, -1) >= i; i += 1) {
        lua_pushcfunction(L, uielement3d_display, NULL);
        lua_rawgeti(L, -2, i);
	lua_pushvalue(L, 2);
	lua_call(L, 2, 0);
    }
    
    return 0;
}

static int uielement3d_drawVisualsClosure(lua_State* L)
{
    lua_pushcfunction(L, uielement3d_drawVisuals, "uielement3d.drawVisuals");
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushvalue(L, 1);
    lua_call(L, 2, 0);
    return 0;
}


static int uielement3d_renderHooks(lua_State* L)
{
    luaL_getmetatable(L, "uielement3d");
    lua_getfield(L, -1, "__renderHooks");

    if (!lua_toboolean(L, -1)) {
        lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnRenderGame");
        lua_pushstring(L, "uielement3d");
        lua_pushunsigned(L, 1000);
        lua_pushcclosure(L, uielement3d_drawVisualsClosure, "uielement.drawVisualsClosure", 1);
        lua_call(L, 3, 0);
    }

    lua_pushvalue(L, 1);
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "__renderHooks");
    
    return 0;
}

static int uielement3d__index(lua_State* L)
{
    const char*  k = lua_tostring(L, -1);

    //luaL_getmetatable(L, "uielement3d");
    lua_getmetatable(L, 1);
    lua_getfield(L, -1, k);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
    } else {
        return 1;
    }

    return 0;
}

static void uielement3d_destructor(lua_State* L, void* ud)
{
    std::destroy_at(static_cast<uielement3d*>(ud));    
}

static const luaL_Reg uielement3d_methods[]
{
    {"new", uielement3d_new},
    {"display", uielement3d_display},

    {"drawVisuals", uielement3d_drawVisuals},

    {"renderHooks", uielement3d_renderHooks},
    
    {"__index", uielement3d__index},
    
    {NULL, NULL},
};

int luaopen_uielement3d(lua_State* L)
{
    luaL_newmetatable(L, "uielement3d");
    luaL_registerwithclosure(L, NULL, uielement3d_methods, 1);

    lua_pushstring(L, "uielement3d");
    lua_setfield(L, -2, "__type");

    lua_pushboolean(L, false);
    lua_setfield(L, -2, "__renderHooks");
    
    lua_setuserdatametatable(L, 2);
    lua_setuserdatadtor(L, 2, uielement3d_destructor);

    lua_newtable(L);

    lua_pushstring(L, "UIElement3DManager");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIVisual3DManager");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIVisual3DManagerViewport");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
    
    luaL_getmetatable(L, "uielement3d");
    lua_setmetatable(L, -2);
    
    return 1;
}
