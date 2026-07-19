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
    // TODO:
    std::shared_ptr<UIElement> parent;
    // TODO:
    std::vector<std::shared_ptr<UIElement>> child;

    uint32_t globalId = (uint32_t)RENDER_FG_GLOBALID;

    Rectangle rec = {0, 0, 1, 1};

    UIShapeType shapeType = SQUARE;
  
    float rounded = 0.1;

    Color uiColor = BLACK;

    Color bgColor = WHITE;
    Color hoverColor = WHITE;
    Color pressedColor = WHITE;

    Color borderColor = WHITE;
    
    Image bgImage;
    Texture bgImageTexture;
    Color bgImageColor = WHITE;

    bool interactive = false;
  
    bool customDisplayOnly = false;
  
    bool displayed = false;
    bool destroyed = false;

    bool noReload = false;
  
    void display();
    bool show(bool forceReload);
    void hide(bool noreload);
    void reload();
    void kill();

    void uiText(const char* str, uint32_t x, uint32_t y);

    ~UIElement();
};

//TODO:
template<typename T, typename U>
using u_map = std::unordered_map<T, U>;
//TODO:
template<typename T>
using vec = std::vector<T>;

static u_map<uint32_t, vec<std::shared_ptr<UIElement>>> UIVisualManager;

static vec<std::shared_ptr<UIElement>> UIElementManager;

static vec<std::shared_ptr<UIElement>> UIMouseHandler;

static vec<std::shared_ptr<UIElement>> UIKeyboardHandler;

void UIElement::display()
{
    if (shapeType == SQUARE) {
        DrawRectangleRec(rec, bgColor);
	if (borderColor.a > 0)
	    DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, borderColor);
    }

    if (shapeType == ROUNDED)
        DrawRectangleRounded(rec, rounded, 8, bgColor);

    if (bgImage.data)
        DrawTexture(bgImageTexture, rec.x, rec.y, bgImageColor);
}

bool UIElement::show(bool forceReload)
{
    if (noReload && !forceReload) {
        return displayed;
    } else if (forceReload) {
        noReload = false;
    }
    
    /*
    auto& v = UIVisualManager[globalId];

    int num = -1;
    
    if (num == -1) {
      if (interactive || keyboard)
	activate()
    }
    */
    
    for (std::shared_ptr<UIElement> elem : child) {
        elem->show(false);
    }

    // call onShow()
    
    displayed = true;
    return displayed;
}

void UIElement::hide(bool noreload)
{
    for (std::shared_ptr<UIElement> elem : child) {
        elem->hide(false);
    }
    
    noReload = noreload;

    if (destroyed || !displayed) return;

    //if (interactive || keyboard) deactivate();
    
    displayed = false;
}

void UIElement::reload()
{
    hide(false);
    show(false);
}

void UIElement::kill()
{
    for (std::shared_ptr<UIElement> elem : child) {
        elem->kill();
    }
    
    destroyed = true;
}

void UIElement::uiText(const char* str, uint32_t x, uint32_t y)
{
    DrawText(str, rec.x + x, rec.y + y, 20, uiColor);
}

UIElement::~UIElement()
{

    if (bgImage.data) {
        UnloadImage(bgImage);
        UnloadTexture(bgImageTexture);
    }

    std::cout << "~UIElement: " << this << std::endl;
}

static void UIElement_destructor(lua_State* L, void* ud)
{
    std::shared_ptr<UIElement>* elem = static_cast<std::shared_ptr<UIElement>*>(ud);
    std::destroy_at(elem);
    std::cout << "~uielement: " << *elem << " count: " << elem->use_count() << std::endl;
}

static int UIElement_display(lua_State* L)
{
    std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1));

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
    std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1));

    bool res = (*ud)->show(false);

    lua_pushlightuserdata(L, ud->get());
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "onShow");
    if(lua_isfunction(L, -1)) {
        lua_call(L, 0, 0);
    }

    lua_pushboolean(L, res);
    
    return 1;
}

static int UIElement_hide(lua_State* L)
{
    (*static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1)))->hide(false);
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

#include <cstring>
#define lua_isuielement(state, idx) (lua_isuserdata(state, idx) && strcmp(lua_tostring(L, -!(!lua_getfield(L, -1, "__type"))), "uielement") == 0)

static void parseColor(lua_State* L, Color* color, const char* name)
{
    lua_getfield(L, -2, name);
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        color->r = 255 * lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        color->g = 255 * lua_tointeger(L, -1);
        lua_rawgeti(L, -3, 3);
        color->b = 255 * lua_tointeger(L, -1);
        lua_rawgeti(L, -4, 4);
        color->a = 255 * lua_tointeger(L, -1);
        lua_pop(L, 5);
    } else {
        lua_pop(L, 1);
    }
}

static int UIElement_new(lua_State* L)
{
    int nargs = lua_gettop(L);

    std::shared_ptr<UIElement>& elem = *static_cast<std::shared_ptr<UIElement>*>(lua_newuserdatataggedwithmetatable(L, sizeof(std::shared_ptr<UIElement>), 1));
    elem = std::make_shared<UIElement>();
    
    lua_pushlightuserdata(L, elem.get());
    lua_newtable(L);
    lua_pushvalue(L, -3);
    lua_setfield(L, -2, "__self");
    lua_settable(L, LUA_REGISTRYINDEX);
    
    UIVisualManager[elem->globalId].push_back(elem);
    
    if (!nargs && !lua_istable(L, 1)) {
        std::cout << "error: uielement.new(table)" << std::endl;
        return 1;
    }

    lua_getfield(L, -2, "parent");
    if (lua_isuserdata(L, -1)) {
        elem->parent = *static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, -1));
	lua_pop(L, 2);
    } else {
        if (!lua_isnil(L, -1)) std::cout << "error: field parent is not of type uielement" << std::endl;
	UIElementManager.push_back(elem);
        lua_pop(L, 1);
    }

    lua_getfield(L, -2, "interactive");
    if (lua_isboolean(L, -1)) {
        elem->interactive = lua_toboolean(L, -1);
	UIMouseHandler.push_back(elem);
    }
    lua_pop(L, 1);

    std::cout << "uielement: " << elem << " count: " << elem.use_count() << std::endl;
    
    lua_getfield(L, -2, "globalId");
    if (lua_isnumber(L, -1)) {
        elem->globalId = lua_tounsigned(L, -1);	
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

    lua_getfield(L, -2, "rec");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        elem->rec.x = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        elem->rec.y = lua_tointeger(L, -1);
        lua_rawgeti(L, -3, 3);
        elem->rec.width = lua_tointeger(L, -1);
        lua_rawgeti(L, -4, 4);
        elem->rec.height = lua_tointeger(L, -1);
        lua_pop(L, 5);
    } else {
        lua_pop(L, 1);
    }

    parseColor(L, &elem->uiColor, "uiColor");
    parseColor(L, &elem->bgColor, "bgColor");
    parseColor(L, &elem->hoverColor, "hoverColor");
    parseColor(L, &elem->pressedColor, "pressedColor");
    parseColor(L, &elem->borderColor, "borderColor"); 

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

    elem->displayed = true;

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
        lua_pushlightuserdata(L, elem.get());
	lua_gettable(L, LUA_REGISTRYINDEX);
	lua_getfield(L, -1, "__self");

	lua_pushcfunction(L, UIElement_display, NULL);
        lua_pushvalue(L, -2);
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

static int UIElement_handleMouseHover(lua_State* L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);

    for (int i = UIMouseHandler.size(); i > 0; i -= 1) {
        auto& elem = UIMouseHandler[i - 1];
	if((x >= elem->rec.x && x <= (elem->rec.x + elem->rec.width)) &&
	   (y >= elem->rec.y && y <= (elem->rec.y + elem->rec.height))) {
	    elem->bgColor = elem->hoverColor;
	}
    }
    
    return 0;
}

static int UIElement_mouseHooks(lua_State* L)
{
    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, "__mouseHooks");

    if (!lua_toboolean(L, -1)) {
        lua_pushcfunction(L, Api_SetHook, NULL);
        lua_pushstring(L, "OnMouseMoved");
        lua_pushstring(L, "uielement");
        lua_pushcfunction(L, UIElement_handleMouseHover, NULL);
        lua_call(L, 3, 0);
    }

    lua_pushvalue(L, 1);
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "__mouseHooks");

    return 0;
}

static int UIElement_keyboardHooks(lua_State* L)
{
    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, "__keyboardHooks");

    if (!lua_toboolean(L, -1)) {
    }

    lua_pushvalue(L, 1);
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "__keyboardHooks");

    return 0;
}

static int UIElement_renderHooks(lua_State* L)
{
    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, "__renderHooks");

    if (!lua_toboolean(L, -1)) {
        lua_pushcfunction(L, Api_SetHook, NULL);
        lua_pushstring(L, "OnRenderForeground");
        lua_pushstring(L, "uielement");
        lua_pushunsigned(L, (uint32_t)RENDER_FG_GLOBALID);
        lua_pushcclosure(L, UIElement_drawVisualsClosure, "UIElement.drawVisualsClosure", 1);
        lua_call(L, 3, 0);

        lua_pushcfunction(L, Api_SetHook, NULL);
        lua_pushstring(L, "OnRenderBackground");
        lua_pushstring(L, "uielement");
        lua_pushunsigned(L, (uint32_t)RENDER_BG_GLOBALID);
        lua_pushcclosure(L, UIElement_drawVisualsClosure, "UIElement.drawVisualsClosure", 1);
        lua_call(L, 3, 0);
    }

    lua_pushvalue(L, 1);
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "__renderHooks");
    
    return 0;
}

static int UIElement__index(lua_State* L)
{
    const char*  k = lua_tostring(L, -1);

    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, k);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
    } else {
        return 1;
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

static int UIElement_get_MOUSE_X(lua_State* L)
{
    lua_pushnumber(L, GetMouseX());
    return 1;
}

static int UIElement_get_MOUSE_Y(lua_State* L)
{
    lua_pushnumber(L, GetMouseY());
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

    int customDisplayFunc = 2;
    
    if (lua_isboolean(L, 2)) {
        (*ud)->customDisplayOnly = lua_toboolean(L, 2);
	customDisplayFunc += 1;
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

static int UIElement_uiText(lua_State* L)
{
    std::shared_ptr<UIElement>* ud = static_cast<std::shared_ptr<UIElement>*>(lua_touserdata(L, 1));

    const char* string = nullptr;

    if (lua_isstring(L, 2)) {
        string = lua_tostring(L, 2);
    }

    int x = 0;
    int y = 0;

    if (lua_isstring(L, 3)) {
        x = lua_tounsigned(L, 3);
    }

    if (lua_isstring(L, 4)) {
        y = lua_tounsigned(L, 4);
    }
    
    (*ud)->uiText(string, x, y);
    return 0;
}

static int UIElement_uiTextClosure(lua_State* L)
{
    lua_pushcfunction(L, UIElement_uiText, NULL);   
    int n = 0;
    do {
       n += 1;
       lua_pushvalue(L, lua_upvalueindex(n));
    } while (!lua_isnil(L, n));
    lua_call(L, n, 0);
    return 0;
}

static int UIElement_addAdaptedText(lua_State* L)
{
    int nargs = lua_gettop(L);
    lua_pushcfunction(L, UIElement_addCustomDisplay, NULL);
    lua_pushvalue(L, 1);
    for (int n = 1; n <= nargs; n += 1) lua_pushvalue(L, n);
    lua_pushcclosure(L, UIElement_uiTextClosure, NULL, nargs);
    lua_call(L, 2, 0);
    return 0;
}

static const luaL_Reg ApiUIElement[]
{
    {"new", UIElement_new},
    
    {"display", UIElement_display},
    {"show", UIElement_show},
    {"hide", UIElement_hide},
    {"kill", UIElement_kill},

    {"uiText", UIElement_uiText},

    {"addCustomDisplay", UIElement_addCustomDisplay},
    {"addAdaptedText", UIElement_addAdaptedText},
    
    {"drawVisuals", UIElement_drawVisuals},

    {"mouseHooks", UIElement_mouseHooks},
    {"keyboardHooks", UIElement_keyboardHooks},
    {"renderHooks", UIElement_renderHooks},

    {NULL, NULL},
};

static const struct { const char* name; float color[4]; } UIColors[] {
    {"UICOLORBLANK", {0.0, 0.0, 0.0, 0.0}},
    {"UICOLORBLACK", {0.0, 0.0, 0.0, 1.0}},
    {"UICOLORWHITE", {1.0, 1.0, 1.0, 1.0}},
    {"UICOLORRED",   {1.0, 0.0, 0.0, 1.0}},
    {"UICOLORGREEN", {0.0, 1.0, 0.0, 1.0}},
    {"UICOLORBLUE",  {0.0, 0.0, 1.0, 1.0}},
};

static const struct { const char* name; uint32_t context; } UIRenderingContexts[] {
    {"RENDER_FG_GLOBALID", (uint32_t)RENDER_FG_GLOBALID},
    {"RENDER_BG_GLOBALID", (uint32_t)RENDER_BG_GLOBALID},
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
    lua_pushcfunction(L, UIElement_get_MOUSE_X, NULL);
    lua_setfield(L, -2, "MOUSE_X");
    lua_pushcfunction(L, UIElement_get_MOUSE_Y, NULL);
    lua_setfield(L, -2, "MOUSE_Y");
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

    lua_pushboolean(L, false);
    lua_setfield(L, -2, "__mouseHooks");
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "__keyboardHooks");
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "__renderHooks");

    lua_setuserdatametatable(L, 1);
    lua_setuserdatadtor(L, 1, UIElement_destructor);

    lua_newtable(L);

    for (const auto& [name, color] : UIColors) {
        lua_newtable(L);
	lua_pushnumber(L, color[0]);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, color[1]);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, color[2]);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, color[3]);
	lua_rawseti(L, -2, 4);
	lua_setfield(L, -2, name);
    }

    for (const auto& [name, context] : UIRenderingContexts) {
        lua_pushunsigned(L, context);
        lua_setfield(L, -2, name);
    }

    luaL_getmetatable(L, "uielement");
    lua_setmetatable(L, -2);
    
    return 1;
}

