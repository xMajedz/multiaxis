#include "Api.h"

#include "Renderer.h"

#include "raylib.h"

#define lua_isuielement3d(L, idx) (lua_isuserdata(L, idx) && lua_userdatatag(L, idx) == 2)

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

    Shader shader;
  
    bool viewport = false;
  
    Api* ApiInstance;

    void display(Renderer* RendererInstance);
  
    uielement3d(Api* ApiInst);
    ~uielement3d();
};

void uielement3d::display(Renderer* RendererInstance)
{
    RendererInstance->Draw((int)shapeType, size, bgColor);
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

    uielement3d* elem = new (static_cast<uielement3d*>(lua_newuserdatataggedwithmetatable(L, sizeof(uielement3d), 2))) uielement3d(ApiInstance);

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

static int uielement3d_display(lua_State* L)
{
    Renderer* RendererInstance = static_cast<Renderer*>(lua_tolightuserdata(L, 2));
    static_cast<uielement3d*>(lua_touserdata(L, 1))->display(RendererInstance);
    return 0;
}

static int uielement3d__index(lua_State* L)
{
    const char*  k = lua_tostring(L, -1);

    luaL_getmetatable(L, "uielement3d");
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
    
    {"__index", uielement3d__index},
    
    {NULL, NULL},
};

int luaopen_uielement3d(lua_State* L)
{
    luaL_newmetatable(L, "uielement3d");
    luaL_registerwithclosure(L, NULL, uielement3d_methods, 1);

    lua_pushstring(L, "uielement3d");
    lua_setfield(L, -2, "__type");

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
