#include "Api.h"

#include "raylib.h"

#include "raymath.h"

#include <cstring>

enum UIRenderingContext { RENDER_FG_GLOBALID = 1000, RENDER_BG_GLOBALID = 1001 };

enum UIShapeType { SQUARE = 1, ROUNDED = 2 };

enum UITextAlign {
    TEXT_ALIGN_LEFT   = 0,
    TEXT_ALIGN_TOP    = 0,
    TEXT_ALIGN_CENTRE = 1,
    TEXT_ALIGN_MIDDLE = 1,
    TEXT_ALIGN_RIGHT  = 2,
    TEXT_ALIGN_BOTTOM = 2
};

struct uielement
{
    uielement* parent = nullptr;

    uint32_t globalId = (uint32_t)RENDER_FG_GLOBALID;

    Rectangle rec = {0, 0, 1, 1};

    Vector2 shift = {0, 0};

    UIShapeType shapeType = SQUARE;
  
    float rounded = 0.1;

    Color uiColor = WHITE;
    Color uiShadowColor = BLACK;

    Color bgColor = WHITE;
    Color hoverColor = WHITE;
    Color pressedColor = WHITE;

    Color borderColor = BLANK;
    Color borderHoverColor = BLANK;
    
    Image bgImage = {0};
    Texture bgImageTexture = {0};
    Color bgImageColor = WHITE;
  
    bool hoverState = false;
    bool pressedState = false;

    bool interactive = false;
    bool keyboard = false;
  
    bool customDisplayOnly = false;
  
    bool displayed = false;
    bool destroyed = false;

    bool noReload = false;

    bool __positionDirty = true;

    Api* ApiInstance;

    void updateChildPos(lua_State* L);
    void updatePos(lua_State* L);
    void display(lua_State* L);
    bool show(lua_State* L, bool forceReload);
    void hide(lua_State* L, bool noreload);
    void reload(lua_State* L);
    void kill(lua_State* L);

    void uiText(const char* str, uint32_t x, uint32_t y, Font font, UITextAlign hAlign, UITextAlign vAlign, int scale, Color col1, Color col2);

    uielement(lua_State* L);
    ~uielement();
};

#define lua_newuielement(L) static_cast<uielement*>(lua_newuserdatataggedwithmetatable(L, sizeof(uielement), 1))
#define lua_isuielement(L, idx) (lua_isuserdata(L, idx) && lua_userdatatag(L, idx) == 1)
#define lua_touielement(L, idx) static_cast<uielement*>(lua_touserdatatagged(L, idx, 1))

void uielement::updateChildPos(lua_State* L)
{
    if (!__positionDirty)
        return;

    if (shift.x < 0)
        rec.x = parent->rec.x + parent->rec.width + shift.x;
    else
        rec.x = parent->rec.x + shift.x;
    
    if (shift.y < 0)
        rec.y = parent->rec.y + parent->rec.height + shift.y;
    else
        rec.y = parent->rec.y + shift.y;

    __positionDirty = false;
}

void uielement::updatePos(lua_State* L)
{
    if (parent)
        updateChildPos(L);

    lua_pushlightuserdata(L, this);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "child");
    if (!lua_isnil(L, -1)) {
        for (int i = 1; lua_objlen(L, -1) >= i; i += 1) {
	    lua_rawgeti(L, -1, i);
            lua_touielement(L, -1)->updatePos(L);
	    lua_pop(L, 1);
	}
    }
    lua_pop(L, 2);
}
#include <iostream>
void uielement::display(lua_State* L)
{
    lua_pushlightuserdata(L, this);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "customDisplayBefore");
    if (lua_isfunction(L, -1)) {
        lua_call(L, 0, 0);
    } else {
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    
    if (!customDisplayOnly) { 
        if (shapeType == SQUARE) {
            Color col = bgColor;
            Color borderCol = borderColor;
	
	    if (hoverState) {
                col = hoverColor;
	        borderCol = borderHoverColor;
	    }
	
	    if (pressedState) {
                col = pressedColor;
	        //borderCol = borderHoverColor;
	    }

	    if (col.a > 0)
	        DrawRectangleRec(rec, col);

	    if (borderColor.a > 0)
	        DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, borderCol);
        }

        if (shapeType == ROUNDED)
            DrawRectangleRounded(rec, rounded, 8, bgColor);

        if (bgImage.data)
            DrawTexture(bgImageTexture, rec.x, rec.y, bgImageColor);
    }
    
    lua_pushlightuserdata(L, this);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "customDisplay");
    if (lua_isfunction(L, -1)) {
        lua_call(L, 0, 0);
    } else {
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

bool uielement::show(lua_State* L, bool forceReload)
{
    lua_pushlightuserdata(L, this);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "child");
    if (!lua_isnil(L, -1)) {
        for (int i = 1; lua_objlen(L, -1) >= i; i += 1) {
	    lua_rawgeti(L, -2, i);
	    lua_touielement(L, -1)->show(L, false);
	    lua_pop(L, 1);
        }
    }    
    
    if (noReload && !forceReload)
        return displayed;
    else if (forceReload)
        noReload = false;
    
    lua_pushstring(L, "UIVisualManager");
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_pushunsigned(L, globalId);
    lua_gettable(L, -2);
    if (lua_istable(L, -1)) {
        lua_pushvalue(L, 1);
        lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
    }
    
    lua_pushlightuserdata(L, this);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "onShow");
    if (lua_isfunction(L, -1)) {
        lua_call(L, 0, 0);
    }
        
    displayed = true;
    
    return displayed;
}

void uielement::hide(lua_State* L, bool noreload)
{
    lua_pushlightuserdata(L, this);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "child");
    if (!lua_isnil(L, -1)) {
        for (int i = 1; lua_objlen(L, -1) >= i; i += 1) {
	    lua_rawgeti(L, -2, i);
	    lua_touielement(L, -1)->hide(L, false);
	    lua_pop(L, 1);
        }
    }    

    noReload = noreload;
    
    if (destroyed || !displayed) return;
    
    lua_pushstring(L, "UIVisualManager");
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_pushunsigned(L, globalId);
    lua_gettable(L, -2);
    if (lua_istable(L, -1)) {
        lua_newtable(L);
	int j = 1;
	for (int i = 1; i <= lua_objlen(L, -2); i += 1) { 
             lua_rawgeti(L, -2, i);
	     if (this != lua_touielement(L, -1)) {
	         lua_pushvalue(L, -2);
	         lua_pushvalue(L, -2);
	         lua_rawseti(L, -2, j);
	         lua_pop(L, 1);
	         j += 1;
	     }
	     lua_pop(L, 1);
	}
        lua_pushunsigned(L, globalId);
	lua_pushvalue(L, -2);
	
	lua_settable(L, -5);
    }
    
    displayed = false;
}

void uielement::reload(lua_State* L)
{
    hide(L, false);
    show(L, false);
}

void uielement::kill(lua_State* L)
{
    lua_pushlightuserdata(L, this);
    lua_gettable(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "child");
    if (!lua_isnil(L, -1)) {
        for (int i = 1; lua_objlen(L, -1) >= i; i += 1) {
            lua_rawgeti(L, -1, i);
	    lua_touielement(L, -1)->kill(L);
        }
    } 

    destroyed = true;
}

void uielement::uiText(
    const char* str,
    uint32_t x, uint32_t y,
    Font font,
    UITextAlign hAlign, UITextAlign vAlign,
    int scale,
    Color col1, Color col2
    )
{
    Vector2 size = MeasureTextEx(font, str, (float)scale, scale * 0.1f);

    Vector2 pos {
        x + rec.x + Lerp(0.0f, rec.width  - size.x, ((float)hAlign) * 0.5f),
        y + rec.y + Lerp(0.0f, rec.height - size.y, ((float)vAlign) * 0.5f),
    };
	    
    DrawTextEx(font, str, pos, scale, 1, col1);
    //DrawTextEx(font, str, pos, scale, 1, col2);
}

static void parseColor(lua_State* L, Color* color, const char* name)
{
    lua_getfield(L, -2, name);
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        color->r = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, -2, 2);
        color->g = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, -3, 3);
        color->b = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, -4, 4);
        color->a = 255 * lua_tonumber(L, -1);
        lua_pop(L, 4); // pop [r, g, b, a]
    }
    lua_pop(L, 1); // pop [table]
}

uielement::uielement(lua_State* L)
{
    ApiInstance = static_cast<Api*>(lua_tolightuserdata(L, lua_upvalueindex(1)));

    lua_pushlightuserdata(L, this);
    lua_newtable(L);
    lua_newtable(L);
    lua_setfield(L, -2, "child");
    lua_settable(L, LUA_REGISTRYINDEX);
    
    if (!lua_gettop(L) || !lua_istable(L, 1)) {
        ApiInstance->Log("error: uielement.new(o: table)");
        return;
    }

    lua_getfield(L, -2, "parent");
    if (lua_isuielement(L, -1)) {
        parent = lua_touielement(L, -1);

	globalId = parent->globalId;
	
	uiColor = parent->uiColor;
	uiShadowColor = parent->uiShadowColor;
    }
    lua_pop(L, 1);

    lua_getfield(L, -2, "globalId");
    if (lua_isnumber(L, -1)) {
        globalId = lua_tounsigned(L, -1);	
    }
    lua_pop(L, 1);

    lua_getfield(L, -2, "interactive");
    if (lua_isboolean(L, -1)) {
        interactive = lua_toboolean(L, -1);

	lua_pushstring(L, "UIMouseHandler");
        lua_gettable(L, LUA_REGISTRYINDEX);
	
        lua_pushvalue(L, 2);
        lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -2, "keyboard");
    if (lua_isboolean(L, -1)) {
        interactive = lua_toboolean(L, -1);

	lua_pushstring(L, "UIKeyboardHandler");
        lua_gettable(L, LUA_REGISTRYINDEX);
	
        lua_pushvalue(L, 2);
        lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -2, "pos");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        if (parent)
	    shift.x = lua_tointeger(L, -1);
	else
	    rec.x = lua_tointeger(L, -1);

	lua_rawgeti(L, -2, 2);
	if (parent)
	    shift.y = lua_tointeger(L, -1);
	else
            rec.y = lua_tointeger(L, -1);

	lua_pop(L, 2);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -2, "size");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
        rec.width = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        rec.height = lua_tointeger(L, -1);
        lua_pop(L, 2);
    }
    lua_pop(L, 1);

    lua_getfield(L, -2, "rec");
    if (lua_istable(L, -1)) {
        lua_rawgeti(L, -1, 1);
	if (parent)
	    shift.x = lua_tointeger(L, -1);
	else
            rec.x = lua_tointeger(L, -1);

	lua_rawgeti(L, -2, 2);
	if (parent)
	    shift.y = lua_tointeger(L, -1);
	else
            rec.y = lua_tointeger(L, -1);

	lua_rawgeti(L, -3, 3);
        rec.width = lua_tointeger(L, -1);
        lua_rawgeti(L, -4, 4);
        rec.height = lua_tointeger(L, -1);
        lua_pop(L, 4);
    }
    lua_pop(L, 1);

    parseColor(L, &uiColor, "uiColor");

    parseColor(L, &bgColor, "bgColor");
    parseColor(L, &hoverColor, "hoverColor");
    parseColor(L, &pressedColor, "pressedColor");

    parseColor(L, &borderColor, "borderColor"); 
    parseColor(L, &borderHoverColor, "borderHoverColor"); 

    lua_getfield(L, -2, "bgImage");
    if (lua_isstring(L, -1)) {
        bgImage = LoadImage(lua_tostring(L, -1));
        bgImageTexture = LoadTextureFromImage(bgImage);
	rec.width  = bgImage.width;
	rec.height = bgImage.height;
    }
    lua_pop(L, 1);

    lua_getfield(L, -2, "shapeType");
    if (lua_isnumber(L, -1)) {
        shapeType = (UIShapeType)lua_tounsigned(L, -1);
    }
    lua_pop(L, 1);

    if (parent) {
	lua_pushlightuserdata(L, parent);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_getfield(L, -1, "child");
	lua_pushvalue(L, 2);
        lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
	lua_pop(L, 2);    
    } else {
      	lua_pushstring(L, "UIElementManager");
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, 2);
        lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
        lua_pop(L, 1);
    }
    
    lua_pushstring(L, "UIVisualManager");
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_pushunsigned(L, globalId);
    lua_gettable(L, -2);
    if (lua_isnil(L, -1)) {
	lua_pop(L, 1);
	
        lua_newtable(L);

	lua_pushvalue(L, -2);
	lua_pushunsigned(L, globalId);
	lua_pushvalue(L, -3);
	lua_settable(L, -3);
        lua_pop(L, 1);
    }
    lua_pushvalue(L, 2);
    lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
    
    lua_pop(L, 2);

    updatePos(L);
    
    displayed = true;
    
    ApiInstance->Log(TextFormat("uielement.new: %p", this));
}

uielement::~uielement()
{
    if (bgImage.data) {
        UnloadImage(bgImage);
	UnloadTexture(bgImageTexture);
    }
    
    ApiInstance->Log(TextFormat("~uielement: %p", this));
}

static void uielement_destructor(lua_State* L, void* ud)
{
    std::destroy_at(static_cast<uielement*>(ud));    
}

static int uielement_updateChildPos(lua_State* L)
{
    lua_touielement(L, 1)->updateChildPos(L);;
    return 0;
}

static int uielement_updatePos(lua_State* L)
{
    lua_touielement(L, 1)->updatePos(L);
    return 0;
}

static int uielement_display(lua_State* L)
{
    lua_touielement(L, 1)->display(L);
    return 0;
}

static int uielement_show(lua_State* L)
{
    bool res = lua_touielement(L, 1)->show(L, false);
    lua_pushboolean(L, res);
    return 1;
}

static int uielement_hide(lua_State* L)
{
    lua_touielement(L, 1)->hide(L, false);
    return 0;
}

static int uielement_reload(lua_State* L)
{
    lua_touielement(L, 1)->reload(L);
    return 0;
}

static int uielement_kill(lua_State* L)
{
    lua_touielement(L, 1)->kill(L);
    return 0;
}

static int uielement_new(lua_State* L)
{
    uielement* elem = new (lua_newuielement(L)) uielement(L);
    return 1;
}

static int uielement_drawVisuals(lua_State* L)
{
    uint32_t globalId;
    
    if (lua_gettop(L) == 0)
        return 0;

    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "globalId");
	if (lua_isnumber(L, -1)) {
	    globalId = lua_tounsigned(L, -1);
	}
	lua_pop(L, 2);
    }
    
    if (lua_isnumber(L, -1)) {
        globalId = lua_tounsigned(L, -1);
    }
      
    lua_pushstring(L, "UIVisualManager");
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_pushunsigned(L, globalId);
    lua_gettable(L, -2);
    if (!lua_istable(L, -1)) {
        return 0;
    }
    
    for (int i = 1; lua_objlen(L, -1) >= i; i += 1) {
        lua_rawgeti(L, -1, i);
	lua_touielement(L, -1)->display(L);
        lua_pop(L, 1);
    }
    
    return 0;
}

static int uielement_drawVisualsClosure(lua_State* L)
{
    lua_pushcfunction(L, uielement_drawVisuals, NULL);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_call(L, 1, 0);
    return 0;
}

static int uielement_handleMouseHover(lua_State* L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);

    lua_pushstring(L, "UIMouseHandler");
    lua_gettable(L, LUA_REGISTRYINDEX);

    for (int i = lua_objlen(L, -1); i > 0; i -= 1) {
        lua_rawgeti(L, -1, i);
	uielement* elem = lua_touielement(L, -1);
	if((x >= elem->rec.x && x <= (elem->rec.x + elem->rec.width)) &&
	   (y >= elem->rec.y && y <= (elem->rec.y + elem->rec.height))) {
	    elem->hoverState = true;
	} else {
	    elem->hoverState = false;
	}
	lua_pop(L, 1);
    }
    
    return 0;
}

static int uielement_handleMouseButtonPressed(lua_State* L)
{
    int btn = lua_tointeger(L, 1);
    float x = lua_tonumber(L, 2);
    float y = lua_tonumber(L, 3);

    lua_pushstring(L, "UIMouseHandler");
    lua_gettable(L, LUA_REGISTRYINDEX);

    for (int i = lua_objlen(L, -1); i > 0; i -= 1) {
        lua_rawgeti(L, -1, i);
	uielement* elem = lua_touielement(L, -1);
        if (elem->hoverState) {
	    elem->pressedState = true;

            lua_pushlightuserdata(L, elem);
            lua_gettable(L, LUA_REGISTRYINDEX);
       
	    lua_getfield(L, -1, "mouseButtonPressed");
            if(lua_isfunction(L, -1)) {
	        lua_pushinteger(L, btn);
                lua_call(L, 1, 0);
            } else {
	        lua_pop(L, 1);
	    }
	    lua_pop(L, 1);
	}
	lua_pop(L, 1);
    }

    return 0;
}

static int uielement_handleMouseButtonReleased(lua_State* L)
{
    int btn = lua_tointeger(L, 1);

    lua_pushstring(L, "UIMouseHandler");
    lua_gettable(L, LUA_REGISTRYINDEX);

    for (int i = lua_objlen(L, -1); i > 0; i -= 1) {
        lua_rawgeti(L, -1, i);
	uielement* elem = lua_touielement(L, -1);
        if (elem->hoverState) elem->pressedState = false;
	lua_pop(L, 1);
    }

    return 0;
}

static int uielement_handleKeyPressed(lua_State* L)
{
    int key = lua_tointeger(L, 1);
    int keyCode = lua_tointeger(L, 2);

    lua_pushstring(L, "UIKeyboardHandler");
    lua_gettable(L, LUA_REGISTRYINDEX);
    
    for (int i = lua_objlen(L, -1); i > 0; i -= 1) {
        lua_rawgeti(L, -1, i);
	uielement* elem = lua_touielement(L, -1);
        lua_pushlightuserdata(L, elem);
        lua_gettable(L, LUA_REGISTRYINDEX);
            
        if (keyCode == KEY_ENTER) {
            lua_getfield(L, -1, "enterAction");
            if(lua_isfunction(L, -1)) {
                lua_call(L, 0, 0);
	    } else {
	        lua_pop(L, 1);
	    }
        }
	
	lua_getfield(L, -1, "keyPressed");
        if(lua_isfunction(L, -1)) {
	    lua_pushinteger(L, key);
	    lua_pushinteger(L, keyCode);
            lua_call(L, 2, 0);
        }
	
	lua_pop(L, 2);
    }
    
    return 0;
}

static int uielement_handleKeyReleased(lua_State* L)
{
    int key = lua_tointeger(L, 1);
    int keyCode = lua_tointeger(L, 2);

    lua_pushstring(L, "UIKeyboardHandler");
    lua_gettable(L, LUA_REGISTRYINDEX);
    
    for (int i = lua_objlen(L, -1); i > 0; i -= 1) {
        lua_rawgeti(L, -1, i);
	uielement* elem = lua_touielement(L, -1);
        lua_pushlightuserdata(L, elem);
        lua_gettable(L, LUA_REGISTRYINDEX);

	lua_getfield(L, -1, "keyReleased");
        if(lua_isfunction(L, -1)) {
	    lua_pushinteger(L, key);
	    lua_pushinteger(L, keyCode);
            lua_call(L, 2, 0);
        }

	lua_pop(L, 2);
    }
    
    return 0;
}

static int uielement_mouseHooks(lua_State* L)
{
    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, "__mouseHooks");

    if (!lua_toboolean(L, -1)) {
        lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnMouseMoved");
        lua_pushstring(L, "uielement");
        lua_pushcfunction(L, uielement_handleMouseHover, NULL);
        lua_call(L, 3, 0);

	lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnMouseButtonPressed");
        lua_pushstring(L, "uielement");
        lua_pushcfunction(L, uielement_handleMouseButtonPressed, NULL);
        lua_call(L, 3, 0);

	lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnMouseButtonReleased");
        lua_pushstring(L, "uielement");
        lua_pushcfunction(L, uielement_handleMouseButtonReleased, NULL);
        lua_call(L, 3, 0);
    }

    lua_pushvalue(L, 1);
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "__mouseHooks");

    return 0;
}

static int uielement_keyboardHooks(lua_State* L)
{
    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, "__keyboardHooks");

    if (!lua_toboolean(L, -1)) {
      	lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnKeyPressed");
        lua_pushstring(L, "uielement");
        lua_pushcfunction(L, uielement_handleKeyPressed, NULL);
        lua_call(L, 3, 0);

	lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnKeyReleased");
        lua_pushstring(L, "uielement");
        lua_pushcfunction(L, uielement_handleKeyReleased, NULL);
        lua_call(L, 3, 0);
    }

    lua_pushvalue(L, 1);
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "__keyboardHooks");

    return 0;
}

static int uielement_renderHooks(lua_State* L)
{
    luaL_getmetatable(L, "uielement");
    lua_getfield(L, -1, "__renderHooks");

    if (!lua_toboolean(L, -1)) {
        lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnRenderForeground");
        lua_pushstring(L, "uielement");
        lua_pushunsigned(L, (uint32_t)RENDER_FG_GLOBALID);
        lua_pushcclosure(L, uielement_drawVisualsClosure, NULL, 1);
        lua_call(L, 3, 0);

        lua_pushcfunction(L, Api_AddHook, NULL);
        lua_pushstring(L, "OnRenderBackground");
        lua_pushstring(L, "uielement");
        lua_pushunsigned(L, (uint32_t)RENDER_BG_GLOBALID);
        lua_pushcclosure(L, uielement_drawVisualsClosure, NULL, 1);
        lua_call(L, 3, 0);
    }

    lua_pushvalue(L, 1);
    lua_pushboolean(L, true);
    lua_setfield(L, -2, "__renderHooks");
    
    return 0;
}

static int uielement__index(lua_State* L)
{
    const char*  k = lua_tostring(L, -1);

    lua_getmetatable(L, 1);
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
	    lua_pushvalue(L, 1);
	    lua_call(L, 1, 1);
	    return 1;
	}
    }

    return 0;
}

static int uielement_get_clock(lua_State* L)
{
    lua_pushnumber(L, GetTime());
    return 1;
}

static int uielement_get_deltaClock(lua_State* L)
{
    lua_pushnumber(L, GetFrameTime());
    return 1;
}

static int uielement_get_hoverState(lua_State* L)
{
    lua_pushboolean(L, lua_touielement(L, 1)->hoverState);
    return 1;
}

static int uielement_get_displayed(lua_State* L)
{
    lua_pushboolean(L, lua_touielement(L, 1)->displayed);
    return 1;
}

static int uielement_get_WIN_W(lua_State* L)
{
    lua_pushnumber(L, GetScreenWidth());
    return 1;
}

static int uielement_get_WIN_H(lua_State* L)
{
    lua_pushnumber(L, GetScreenHeight());
    return 1;
}

static int uielement_get_MOUSE_X(lua_State* L)
{
    lua_pushnumber(L, GetMouseX());
    return 1;
}

static int uielement_get_MOUSE_Y(lua_State* L)
{
    lua_pushnumber(L, GetMouseY());
    return 1;
}

static int uielement__newindex(lua_State* L)
{
    const char*  k = lua_tostring(L, -2);
    lua_getmetatable(L, 1);
    lua_getfield(L, -1, "set");
    lua_getfield(L, -1, k);
    lua_pushvalue(L, -6);
    lua_pushvalue(L, -5);
    lua_call(L, 2, 0);
    return 0;
}

static int uielement_set_mouseButtonPressed(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
        uielement* ud = lua_touielement(L, 1);
        lua_pushlightuserdata(L, ud);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "mouseButtonPressed");
    }

    return 0;
}

static int uielement_set_keyPressed(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
        uielement* ud = lua_touielement(L, 1);
        lua_pushlightuserdata(L, ud);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "keyPressed");
    }

    return 0;
}

static int uielement_set_enterAction(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
        uielement* ud = lua_touielement(L, 1);
        lua_pushlightuserdata(L, ud);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "enterAction");
    }

    return 0;
}

static int uielement_set_onShow(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
        uielement* ud = lua_touielement(L, 1);
        lua_pushlightuserdata(L, ud);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "onShow");
    }

    return 0;
}

static int uielement_set_customDisplay(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
        uielement* ud = lua_touielement(L, 1);
        lua_pushlightuserdata(L, ud);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "customDisplay");
    }

    return 0;
}

static int uielement_set_customDisplayBefore(lua_State* L)
{
    if (lua_isuserdata(L, -2) && lua_isfunction(L, -1)) {
        uielement* ud = lua_touielement(L, 1);
        lua_pushlightuserdata(L, ud);
        lua_gettable(L, LUA_REGISTRYINDEX);
        lua_pushvalue(L, -2);
        lua_setfield(L, -2, "customDisplayBefore");
    }

    return 0;
}

static int uielement_addCustomDisplay(lua_State* L)
{
    uielement* ud = lua_touielement(L, 1);

    int customDisplayFunc = 2;
    
    if (lua_isboolean(L, 2)) {
        ud->customDisplayOnly = lua_toboolean(L, 2);
	customDisplayFunc += 1;
    }

    if (lua_isfunction(L, customDisplayFunc)) {
        lua_pushcfunction(L, uielement_set_customDisplay, NULL);
        lua_pushvalue(L, 1);
        lua_pushvalue(L, customDisplayFunc);
        lua_call(L, 2, 0);
    }
    
    if (lua_isfunction(L, customDisplayFunc + 1)) {
        lua_pushcfunction(L, uielement_set_customDisplayBefore, NULL);
        lua_pushvalue(L, 1);
        lua_pushvalue(L, customDisplayFunc + 1);
        lua_call(L, 2, 0);
    }

    return 0;
}

static int uielement_uiText(lua_State* L)
{
    uielement* ud = lua_touielement(L, 1);

    const char* string = "";
    
    if (lua_isstring(L, 2))
        string = lua_tostring(L, 2);

    int x = 0;
    int y = 0;
 
    if (lua_isnumber(L, 3))
        x = lua_tounsigned(L, 3);

    if (lua_isnumber(L, 4))
        y = lua_tounsigned(L, 4);

    Font font = GetFontDefault();
    
    if (lua_isnumber(L, 5)) {
        font = GetFontDefault();
    }

    UITextAlign hAlign = TEXT_ALIGN_CENTRE;
    UITextAlign vAlign = TEXT_ALIGN_MIDDLE;
  
    if (lua_isnumber(L, 6))
        hAlign = (UITextAlign)lua_tointeger(L, 6);

    if (lua_isnumber(L, 7))
        hAlign = (UITextAlign)lua_tointeger(L, 7);
    
    int scale = 20;
    
    if (lua_isnumber(L, 8))
        scale = lua_tointeger(L, 8);
       
    Color col1 = ud->uiColor;
    Color col2 = ud->uiColor;
    
    if (lua_istable(L, 9)) {
        lua_rawgeti(L, 9, 1);
        col1.r = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 9, 2);
        col1.g = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 9, 3);
        col1.b = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 9, 4);
	col1.a = 255 * lua_tonumber(L, -1);
	lua_pop(L, 5);
    }

    if (lua_istable(L, 10)) {
        lua_rawgeti(L, 10, 1);
        col2.r = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 10, 2);
        col2.g = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 10, 3);
        col2.b = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 10, 4);
	col2.a = 255 * lua_tonumber(L, -1);
	lua_pop(L, 5);
    }
    
    ud->uiText(
	string,
	x, y,
	font,
	hAlign, vAlign,
	scale,
	col1, col2
    );
    
    return 0;
}

static int uielement_uiTextClosure(lua_State* L)
{
    lua_pushcfunction(L, uielement_uiText, NULL);
    int i = 1;

    for (i = 1; !lua_isnil(L, -1); i += 1)
        lua_pushvalue(L, lua_upvalueindex(i));

    lua_call(L, i - 1, 0);
    
    return 0;
}

static int uielement_addAdaptedText(lua_State* L)
{
    int nargs = lua_gettop(L);
    int cnargs = 2;
    int upvalues = nargs;;

    lua_pushcfunction(L, uielement_addCustomDisplay, NULL);
    lua_pushvalue(L, 1);  
    if (lua_isboolean(L, 2)) {
       lua_pushvalue(L, 2);
       cnargs += 1;
       upvalues -= 1;
    }
    
    lua_pushvalue(L, 1);
    for (int n = cnargs; n <= nargs; n += 1)
         lua_pushvalue(L, n);
    
    lua_pushcclosure(L, uielement_uiTextClosure, NULL, upvalues);
    lua_call(L, cnargs, 0);
    return 0;
}

static int uielement_addChild(lua_State* L)
{
    Api* ApiInstance = static_cast<Api*>(lua_tolightuserdata(L, lua_upvalueindex(1)));

    lua_pushvalue(L, 1);
    lua_setfield(L, -2, "parent");

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushcclosure(L, uielement_new, NULL, 1);
    lua_pushvalue(L, 2);
    lua_call(L, 1, 1);
    
    return 1;
}

static const luaL_Reg uielement_methods[]
{
    {"new", uielement_new},
    
    {"display", uielement_display},
    {"show", uielement_show},
    {"hide", uielement_hide},
    {"kill", uielement_kill},

    {"uiText", uielement_uiText},

    {"addChild", uielement_addChild},
    {"addCustomDisplay", uielement_addCustomDisplay},
    {"addAdaptedText", uielement_addAdaptedText},
    
    {"drawVisuals", uielement_drawVisuals},

    {"mouseHooks", uielement_mouseHooks},
    {"keyboardHooks", uielement_keyboardHooks},
    {"renderHooks", uielement_renderHooks},

    {"__index", uielement__index},
    {"__newindex", uielement__newindex},

    {NULL, NULL},
};

static const luaL_Reg uielement_getters[]
{
    {"clock",      uielement_get_clock},
    {"deltaClock", uielement_get_deltaClock},
    {"displayed",  uielement_get_displayed},
    {"hoverState", uielement_get_hoverState},
    {"WIN_W",      uielement_get_WIN_W},
    {"WIN_H",      uielement_get_WIN_H},
    {"MOUSE_X",    uielement_get_MOUSE_X},
    {"MOUSE_Y",    uielement_get_MOUSE_Y},
    
    {NULL, NULL},
};

static const luaL_Reg uielement_setters[]
{
    {"onShow",              uielement_set_onShow},
    {"enterAction",         uielement_set_enterAction},
    {"keyPressed",          uielement_set_keyPressed},
    {"mouseButtonPressed",  uielement_set_mouseButtonPressed},
    {"customDisplay",       uielement_set_customDisplay},
    {"customDisplayBefore", uielement_set_customDisplayBefore},
    
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

static const struct { const char* name; uint32_t shape; } UIShapeTypes[] {
    {"SHAPE_SQUARE",  (uint32_t)SQUARE },
    {"SHAPE_ROUNDED", (uint32_t)ROUNDED},
};

static const struct { const char* name; uint32_t context; } UIRenderingContexts[] {
    {"RENDER_FG_GLOBALID", (uint32_t)RENDER_FG_GLOBALID},
    {"RENDER_BG_GLOBALID", (uint32_t)RENDER_BG_GLOBALID},
};

void lua_close_uielement(lua_State* L)
{
    lua_pushstring(L, "UIVisualManager");
    lua_pushnil(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIElementManager");
    lua_pushnil(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIMouseHandler");
    lua_pushnil(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIKeyboardHandler");
    lua_pushnil(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

int luaopen_uielement(lua_State* L)
{
    luaL_newmetatable(L, "uielement");
    luaL_registerwithclosure(L, NULL, uielement_methods, 1);

    lua_newtable(L);
    luaL_register(L, NULL, uielement_getters);
    lua_setfield(L, -2, "get");

    lua_newtable(L);
    luaL_register(L, NULL, uielement_setters);
    lua_setfield(L, -2, "set");
    
    lua_pushstring(L, "uielement");
    lua_setfield(L, -2, "__type");

    lua_pushboolean(L, false);
    lua_setfield(L, -2, "__mouseHooks");
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "__keyboardHooks");
    lua_pushboolean(L, false);
    lua_setfield(L, -2, "__renderHooks");

    lua_setuserdatametatable(L, 1);
    lua_setuserdatadtor(L, 1, uielement_destructor);

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

    for (const auto& [name, shape] : UIShapeTypes) {
        lua_pushunsigned(L, shape);
        lua_setfield(L, -2, name);
    }
    
    for (const auto& [name, context] : UIRenderingContexts) {
        lua_pushunsigned(L, context);
        lua_setfield(L, -2, name);
    }

    lua_pushstring(L, "UIVisualManager");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIElementManager");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIMouseHandler");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pushstring(L, "UIKeyboardHandler");
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
    
    luaL_getmetatable(L, "uielement");
    lua_setmetatable(L, -2);
    
    return 1;
}

