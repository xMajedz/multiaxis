#include "Api.h"

#include "raylib.h"

#include <iostream>

#include <unordered_map>

#include <vector>

#include <memory>

enum UIRenderingContext {
    RENDER_FG_GLOBALID = 1,
    RENDER_BG_GLOBALID = 2,
};

enum UIShapeType { SQUARE, ROUNDED };

class UIElement
{
public:
    std::shared_ptr<UIElement> parent;

    std::vector<std::shared_ptr<UIElement>> child;

    uint32_t globalId = (uint32_t)RENDER_FG_GLOBALID;

    Rectangle rec = {0, 0, 1, 1};

    UIShapeType shapeType = SQUARE;
  
    float rounded = 0.1;

    Color bgColor = WHITE;
    
    Image bgImage;
    Texture bgImageTexture;
    Color bgImageColor = WHITE;

    bool customDisplayOnly = false;
  
    bool displayed = false;
    bool destroyed = false;
    
    void display();
    void show();
    void hide();
    void reload();
    void kill();

    ~UIElement();
};

template<typename T, typename U>
using u_map = std::unordered_map<T, U>;

template<typename T>
using vec = std::vector<T>;

using UIElementResource = std::shared_ptr<UIElement>;

static u_map<uint32_t, vec<std::shared_ptr<UIElement>>> UIVisualManager;

static vec<std::shared_ptr<UIElement>> UIElementManager;

void UIElement::display()
{
    if (shapeType == SQUARE)
         DrawRectangleRec(rec, bgColor);

    if (shapeType == ROUNDED)
        DrawRectangleRounded(rec, rounded, 8, bgColor);

    if (bgImage.data)
        DrawTexture(bgImageTexture, rec.x, rec.y, bgImageColor);
}

void UIElement::show()
{
    for (std::shared_ptr<UIElement> elem : child) {
        elem->show();
    }
    
    displayed = true;
}

void UIElement::hide()
{
    for (std::shared_ptr<UIElement> elem : child) {
        elem->hide();
    }
    
    displayed = false;
}

void UIElement::reload()
{
    hide();
    show();
}

void UIElement::kill()
{
    for (std::shared_ptr<UIElement> elem : child) {
        elem->kill();
    }
    
    auto& v = UIVisualManager[globalId];

    //v.erase(std::remove(v.begin(), v.end(), 2), v.end());
    
    destroyed = true;
}

UIElement::~UIElement()
{
    std::cout << "~UIElement: " << this << std::endl;

    if (bgImage.data) {
        UnloadImage(bgImage);
        UnloadTexture(bgImageTexture);
    }
}

static void UIElement_destructor(lua_State* L, void* ud)
{
    std::shared_ptr<UIElement>* elem = static_cast<std::shared_ptr<UIElement>*>(ud);
    std::destroy_at(elem);
    std::cout << "~uielement: " << *elem << " count: " << elem->use_count() << std::endl;
}

static int UIElement_display(lua_State* L)
{
    std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_tolightuserdata(L, 1));
    //std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1));
    // std::cout << ud << std::endl;

    if ((*ud)->destroyed || !(*ud)->displayed) return 0;

    lua_pushlightuserdata(L, ud->get());
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "customDisplayBefore");
    if(lua_isfunction(L, -1)) {
        lua_call(L, 0, 0);
    }
    
    if (!(*ud)->customDisplayOnly) (*ud)->display();
    
    lua_pushlightuserdata(L, ud->get());
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "customDisplay");
    if(lua_isfunction(L, -1)) {
        lua_call(L, 0, 0);
    }
    
    return 0;
}

static int UIElement_show(lua_State* L)
{
    (*static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1)))->show();
    return 0;
}

static int UIElement_hide(lua_State* L)
{
    (*static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1)))->hide();
    return 0;
}

static int UIElement_reload(lua_State* L)
{
    (*static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1)))->reload();
    return 0;
}

static int UIElement_kill(lua_State* L)
{
    (*static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1)))->kill();
    return 0;
}

static int UIElement_new(lua_State* L)
{
    if (lua_gettop(L) == 0 || !lua_istable(L, -1)) {
        lua_pushnil(L);
        return 1;
    }

    std::shared_ptr<UIElement>& elem = *static_cast<std::shared_ptr<UIElement>*>(lua_newuserdatataggedwithmetatable(L, sizeof(std::shared_ptr<UIElement>), 1));
    elem = std::make_shared<UIElement>();

    lua_getfield(L, -2, "parent");
    if (lua_isuserdata(L, -1)) {
        elem->parent = *static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, -1));
	lua_pop(L, 2);
    } else {
        lua_pop(L, 1);
    }
    
    lua_getfield(L, -2, "globalId");
    if (lua_isnumber(L, -1)) {
        elem->globalId = static_cast<UIRenderingContext>(lua_tounsigned(L, -1));	
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -2, "pos");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        elem->rec.x = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        elem->rec.y = lua_tointeger(L, -1);
        lua_pop(L, 3);
    } else {
        lua_pop(L, 1);
    }

    lua_getfield(L, -2, "size");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        elem->rec.width = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        elem->rec.height = lua_tointeger(L, -1);
        lua_pop(L, 3);
    } else {
        lua_pop(L, 1);
    }

    lua_getfield(L, -2, "bgColor");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        elem->bgColor.r = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        elem->bgColor.g = lua_tointeger(L, -1);
        lua_rawgeti(L, -3, 3);
        elem->bgColor.b = lua_tointeger(L, -1);
        lua_rawgeti(L, -4, 4);
        elem->bgColor.a = lua_tointeger(L, -1);
        lua_pop(L, 5);
    } else {
        lua_pop(L, 1);
    }

    lua_getfield(L, -2, "bgImage");
    if (lua_isstring(L, -1)) {
        elem->bgImage = LoadImage("scripts/von/von.png");
        elem->bgImageTexture = LoadTextureFromImage(elem->bgImage);
	elem->rec.width  = elem->bgImage.width;
	elem->rec.height = elem->bgImage.height;
        lua_pop(L, 2);
    } else {
        lua_pop(L, 1);
    }

    if (elem->parent == nullptr) {
        UIElementManager.push_back(elem);
    }

    UIVisualManager[elem->globalId].push_back(elem);

    elem->displayed = true;

    lua_pushlightuserdata(L, elem.get());
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
    
    std::cout << "uielement: " << elem << " count: " << elem.use_count() << std::endl;

    return 1;
}

static int UIElement_drawVisuals(lua_State* L)
{
    uint32_t globalId;
    
    if (lua_gettop(L) == 0) return 0;

    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "globalId");
	if (lua_isnumber(L, -1)) {
	    globalId = lua_tounsigned(L, -1);
	    lua_pop(L, 2);
	} else {
	    lua_pop(L, 1);
	}
    }
    
    if (lua_isnumber(L, -1)) {
        globalId = lua_tounsigned(L, -1);
    }
    
    for (auto elem : UIVisualManager[globalId]) {
        lua_pushcfunction(L, UIElement_display, NULL);
        lua_pushlightuserdata(L, &elem);
	lua_call(L, 1, 0);
    }
    
    return 0;
}

static int UIElement_drawVisualsClosure(lua_State* L)
{
    lua_pushcfunction(L, UIElement_drawVisuals, "UIElement.drawVisuals");
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_call(L, 1, 0);
    return 0;
}

int Api_SetHook(lua_State* L);

static int UIElement_renderHooks(lua_State* L)
{
    lua_pushcfunction(L, Api_SetHook, NULL);
    lua_pushstring(L, "OnRenderForeground");
    lua_pushstring(L, "uielement");
    lua_pushunsigned(L, 1);
    lua_pushcclosure(L, UIElement_drawVisualsClosure, "UIElement.drawVisualsClosure", 1);
    lua_call(L, 3, 0);

    lua_pushcfunction(L, Api_SetHook, NULL);
    lua_pushstring(L, "OnRenderBackground");
    lua_pushstring(L, "uielement");
    lua_pushunsigned(L, 2);
    lua_pushcclosure(L, UIElement_drawVisualsClosure, "UIElement.drawVisualsClosure", 1);
    lua_call(L, 3, 0);
    return 0;
}

static int UIElement__index(lua_State* L)
{
    const char*  k = lua_tostring(L, -1);

    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, k);
    if (lua_isfunction(L, -1)) {
        return 1;
    } else {
        lua_pop(L, 1);
    }
    
    lua_getfield(L, -1, "get");
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, k);
	if (lua_isfunction(L, -1)) {
	    lua_call(L, 0, 1);
	    return 1;
	}
    }

    return 0;
}

static int UIElement_get_clock(lua_State* L)
{
    lua_pushnumber(L, GetTime());
    return 1;
}

static int UIElement_get_deltaClock(lua_State* L)
{
    lua_pushnumber(L, GetFrameTime());
    return 1;
}

static int UIElement_get_WIN_W(lua_State* L)
{
    lua_pushnumber(L, GetScreenWidth());
    return 1;
}

static int UIElement_get_WIN_H(lua_State* L)
{
    lua_pushnumber(L, GetScreenHeight());
    return 1;
}

static int UIElement__newindex(lua_State* L)
{
    const char*  k = lua_tostring(L, -2);
    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, "set");
    lua_getfield(L, -1, k);
    lua_pushvalue(L, -6);
    lua_pushvalue(L, -5);
    lua_call(L, 2, 0);
    return 0;
}

static int UIElement_set_customDisplay(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
      std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1));
      lua_pushlightuserdata(L, ud->get());
      lua_gettable(L, LUA_REGISTRYINDEX);
      lua_pushvalue(L, -2);
      lua_setfield(L, -2, "customDisplay");
    }

    return 0;
}

static int UIElement_set_customDisplayBefore(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
      std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1));
      lua_pushlightuserdata(L, ud->get());
      lua_gettable(L, LUA_REGISTRYINDEX);
      lua_pushvalue(L, -2);
      lua_setfield(L, -2, "customDisplayBefore");
    }

    return 0;
}

static int UIElement_addCustomDisplay(lua_State* L)
{
    std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1));

    int customDisplayFunc = 0;
    
    if (lua_isfunction(L, 2)) {
        customDisplayFunc = 2;
    } else if (lua_isboolean(L, 2)) {
        (*ud)->customDisplayOnly = lua_toboolean(L, 2);
	customDisplayFunc = 3;
    }

    if (lua_isfunction(L, customDisplayFunc)) {
        lua_pushcfunction(L, UIElement_set_customDisplay, NULL);
        lua_pushvalue(L, 1);
        lua_pushvalue(L, customDisplayFunc);
        lua_call(L, 2, 0);
    }
    
    if (lua_isfunction(L, customDisplayFunc + 1)) {
        lua_pushcfunction(L, UIElement_set_customDisplayBefore, NULL);
        lua_pushvalue(L, 1);
        lua_pushvalue(L, customDisplayFunc + 1);
        lua_call(L, 2, 0);
    }

    return 0;
}

static const luaL_Reg ApiUIElement[]
{
    {"new", UIElement_new},
    
    {"display", UIElement_display},
    {"show", UIElement_show},
    {"hide", UIElement_hide},
    {"kill", UIElement_kill},

    {"addCustomDisplay", UIElement_addCustomDisplay},
    
    {"drawVisuals", UIElement_drawVisuals},

    {NULL, NULL},
};

static const struct UIColor { const char* name; Color color; } UIColors[] {
    {"UICOLORWHITE", {255, 255, 255, 255}},
    {"UICOLORBLACK", {0,   0,   0,   255}},
    {"UICOLORRED",   {255, 0,   0,   255}},
    {"UICOLORGREEN", {0,   255, 0,   255}},
    {"UICOLORBLUE",  {0,   0,   255, 255}},
};

int luaopen_UIElement(lua_State* L)
{
    luaL_newmetatable(L, "uielement");

    luaL_register(L, NULL, ApiUIElement);

    lua_newtable(L);
    lua_pushcfunction(L, UIElement_get_clock, NULL);
    lua_setfield(L, -2, "clock");
    lua_pushcfunction(L, UIElement_get_deltaClock, NULL);
    lua_setfield(L, -2, "deltaClock");
    lua_pushcfunction(L, UIElement_get_WIN_W, NULL);
    lua_setfield(L, -2, "WIN_W");
    lua_pushcfunction(L, UIElement_get_WIN_H, NULL);
    lua_setfield(L, -2, "WIN_H");
    lua_setfield(L, -2, "get");

    lua_newtable(L);
    lua_pushcfunction(L, UIElement_set_customDisplay, NULL);
    lua_setfield(L, -2, "customDisplay");
    lua_pushcfunction(L, UIElement_set_customDisplayBefore, NULL);
    lua_setfield(L, -2, "customDisplayBefore");
    lua_setfield(L, -2, "set");
    
    lua_pushcfunction(L, UIElement__index, NULL);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, UIElement__newindex, NULL);
    lua_setfield(L, -2, "__newindex");
    lua_pushstring(L, "uielement");
    lua_setfield(L, -2, "__type");

    lua_setuserdatametatable(L, 1);
    lua_setuserdatadtor(L, 1, UIElement_destructor);

    UIElement_renderHooks(L);
    
    lua_newtable(L);

    for (const auto& [name, color] : UIColors) {
        lua_newtable(L);
	lua_pushinteger(L, color.r);
	lua_rawseti(L, -2, 1);
	lua_pushinteger(L, color.g);
	lua_rawseti(L, -2, 2);
	lua_pushinteger(L, color.b);
	lua_rawseti(L, -2, 3);
	lua_pushinteger(L, color.a);
	lua_rawseti(L, -2, 4);
	lua_setfield(L, -2, name);
    }

    luaL_getmetatable(L, "uielement");
    lua_setmetatable(L, -2);
    
    return 1;
}

