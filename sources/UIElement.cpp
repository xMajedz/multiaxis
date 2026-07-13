#include "Api.h"

#include "raylib.h"

#include <iostream>

#include <vector>

class UIElement
{
public:
    UIElement* parent = nullptr;

    std::vector<UIElement*> child;

    int posX = 0;
    int posY = 0;
    int width  = 1;
    int height = 1;

    bool displayed = true;
    bool destroyed = false;
  
    Color bgColor = WHITE;
    
    void display();
    void show();
    void hide();
    void reload();
    void kill();

    ~UIElement();
};

static std::vector<UIElement*> UIVisualManager;

void UIElement::display()
{
    if (destroyed || !displayed) return;
  
    DrawRectangle(posX, posY, width, height, bgColor);
}

void UIElement::show()
{
    for (UIElement* elem : UIVisualManager) {
        elem->show();
    }
    
    displayed = true;
}

void UIElement::hide()
{
    for (UIElement* elem : UIVisualManager) {
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
    for (UIElement* elem : UIVisualManager) {
        elem->kill();
    }
    
    destroyed = true;
}

UIElement::~UIElement()
{
    std::cout << "~UIElement:" << this << std::endl;
}

static void UIElement_destructor(lua_State* L, void* ud)
{
    static_cast<UIElement*>(ud)->~UIElement();
}

static int UIElement_display(lua_State* L)
{
    static_cast<UIElement*>(lua_touserdata(L, 1))->display();
    return 0;
}

static int UIElement_show(lua_State* L)
{
    static_cast<UIElement*>(lua_touserdata(L, 1))->show();
    return 0;
}

static int UIElement_hide(lua_State* L)
{
    static_cast<UIElement*>(lua_touserdata(L, 1))->hide();
    return 0;
}

static int UIElement_reload(lua_State* L)
{
    static_cast<UIElement*>(lua_touserdata(L, 1))->reload();
    return 0;
}

static int UIElement_kill(lua_State* L)
{
    static_cast<UIElement*>(lua_touserdata(L, 1))->kill();
    return 0;
}

static int UIElement_new(lua_State* L)
{
    if (lua_gettop(L) == 0 || !lua_istable(L, -1)) {
        lua_pushnil(L);
        return 1;
    }
    
    UIElement o;

    lua_getfield(L, -1, "pos");
    if (lua_istable(L, -1)) {
    lua_rawgeti(L, -1, 1);
    o.posX = lua_tointeger(L, -1);
    lua_rawgeti(L, -2, 2);
    o.posY = lua_tointeger(L, -1);
    lua_pop(L, 3);
    } else {
    lua_pop(L, 1);
    }

    lua_getfield(L, -1, "size");
    if (lua_istable(L, -1)) {
    lua_rawgeti(L, -1, 1);
    o.width = lua_tointeger(L, -1);
    lua_rawgeti(L, -2, 2);
    o.height = lua_tointeger(L, -1);
    lua_pop(L, 3);
    } else {
    lua_pop(L, 1);
    }

    lua_getfield(L, -1, "bgColor");
    if (lua_istable(L, -1)) {
    lua_rawgeti(L, -1, 1);
    o.bgColor.r = lua_tointeger(L, -1);
    lua_rawgeti(L, -2, 2);
    o.bgColor.g = lua_tointeger(L, -1);
    lua_rawgeti(L, -3, 3);
    o.bgColor.b = lua_tointeger(L, -1);
    lua_rawgeti(L, -4, 4);
    o.bgColor.a = lua_tointeger(L, -1);
    lua_pop(L, 5);
    } else {
    lua_pop(L, 1);
    }

    UIElement* elem = static_cast<UIElement*>(lua_newuserdatataggedwithmetatable(L, sizeof(UIElement), 1));

    UIVisualManager.push_back(new (elem) UIElement(o));

    return 1;
}

static int UIElement_drawVisuals(lua_State* L)
{
    for (UIElement* elem : UIVisualManager) {
        elem->display();
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

int Api_SetHook(lua_State* L);

int luaopen_UIElement(lua_State* L)
{
    luaL_newmetatable(L, "UIElement");
    luaL_getmetatable(L, "UIElement");
    lua_setfield(L, -2, "__index");
    lua_pushstring(L, "uielement");
    lua_setfield(L, -2, "__type");

    lua_setuserdatametatable(L, 1);
    lua_setuserdatadtor(L, 1, UIElement_destructor);

    lua_pushstring(L, "RenderForeground");
    lua_pushstring(L, "uielement");
    lua_pushcfunction(L, UIElement_drawVisuals, "UIElement.drawVisuals");

    Api_SetHook(L);    
    
    lua_newtable(L);

    luaL_register(L, NULL, ApiUIElement);

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

    return 1;
}

