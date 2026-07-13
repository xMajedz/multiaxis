#include "Api.h"

#include "raylib.h"

#include <iostream>

class UIElement
{
public:
    int posX = 0;
    int posY = 0;
    int width  = 10;
    int height = 10;

    bool visible = true;
  
    Color bgColor = WHITE;
    
    void display();
    void show();
    void hide();
    void kill();
};

void UIElement::display()
{
    if (visible)
        DrawRectangle(posX, posY, width, height, bgColor);
}

void UIElement::show()
{
    visible = true;
}

void UIElement::hide()
{
    visible = false;
}

void UIElement::kill()
{
    return;
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

static int UIElement_kill(lua_State* L)
{
    static_cast<UIElement*>(lua_touserdata(L, 1))->kill();
    return 0;
}

static int elements;

static int UIElement_new(lua_State* L)
{
    if (lua_gettop(L) == 0 || !lua_istable(L, -1)) {
        lua_pushnil(L);
        return 1;
    }
    
    UIElement o;

    lua_getfield(L, -1, "pos");
    lua_rawgeti(L, -1, 1);
    o.posX = lua_tointeger(L, -1);
    lua_rawgeti(L, -2, 2);
    o.posY = lua_tointeger(L, -1);
    lua_pop(L, 3);

    lua_getfield(L, -1, "size");
    lua_rawgeti(L, -1, 1);
    o.width = lua_tointeger(L, -1);
    lua_rawgeti(L, -2, 2);
    o.height = lua_tointeger(L, -1);
    lua_pop(L, 3);

    lua_getfield(L, -1, "bgColor");
    lua_rawgeti(L, -1, 1);
    o.bgColor.r = lua_tointeger(L, -1);
    lua_rawgeti(L, -2, 2);
    o.bgColor.g = lua_tointeger(L, -1);
    lua_rawgeti(L, -3, 3);
    o.bgColor.b = lua_tointeger(L, -1);
    lua_rawgeti(L, -4, 4);
    o.bgColor.a = lua_tointeger(L, -1);
    lua_pop(L, 5);

    lua_pop(L, 1);

    lua_rawgeti(L, LUA_REGISTRYINDEX, elements);
    int index = lua_objlen(L, -1);
    *static_cast<UIElement*>(lua_newuserdata(L, sizeof(UIElement))) = o;
    lua_rawseti(L, -2, index + 1);
    lua_rawgeti(L, -1, index + 1);
    std::cout << lua_gettop(L) << std::endl;
    luaL_getmetatable(L, "UIElement");
    lua_setmetatable(L, -2);
    
    return 1;
}

static int UIElement_drawVisuals(lua_State* L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, elements);
    lua_pushnil(L);
	
    while (lua_next(L, -2) != 0) {
        //const char* key = lua_tostring(L, -2);
	//static_cast<UIElement*>(lua_touserdata(L, -1))->display();
	//std::cout << lua_gettop(L) << std::endl;
        //lua_pop(L, 2);
	/*if (lua_isfunction(L, -1)) {
	    for (int i = 2; i <= nargs; i += 1) {
	        lua_pushvalue(L, i);
	    }

	    int status = lua_pcall(L, nargs - 1, 0, 0);

	    if (status != LUA_OK) {
	        log(lua_tostring(L, -1));
	        lua_pop(L, nargs - 3);
	    }
	    }*/
    }
	
    lua_pop(L, 1);

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

int luaopen_UIElement(lua_State* L)
{
    lua_newtable(L);
    elements = lua_ref(L, -1);
    lua_pop(L, 1);
    
    luaL_newmetatable(L, "UIElement");
    luaL_getmetatable(L, "UIElement");
    lua_setfield(L, -1, "__index");
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

