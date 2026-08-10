#include "Renderer.h"

#define RendererInstance (static_cast<Renderer*>(lua_tolightuserdata(L, lua_upvalueindex(1))))

int Renderer_DrawText(lua_State* L)
{
    Font font = GetFontDefault();

    if (lua_isnumber(L, 1))
        font = GetFontDefault();

    const char* text = "";

    if (lua_isstring(L, 2))
        text = lua_tostring(L, 2);

    Vector2 position {0};
    
    if (lua_isnumber(L, 3))
        position.x = lua_tonumber(L, 3);

    if (lua_isnumber(L, 4))
        position.y = lua_tonumber(L, 4);

    float fontSize = 20;
    
    if (lua_isnumber(L, 5))
        fontSize = lua_tonumber(L, 5);

    float spacing = 1;
    
    if (lua_isnumber(L, 6))
        spacing = lua_tonumber(L, 6);

    Color color = WHITE;

    if (lua_istable(L, 7)) {
        lua_rawgeti(L, 7, 1);
        color.r = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 7, 2);
        color.g = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 7, 3);
        color.b = 255 * lua_tonumber(L, -1);
        lua_rawgeti(L, 7, 4);
        color.a = 255 * lua_tonumber(L, -1);
    }
    
    RendererInstance->DrawText(font, text, position, fontSize, spacing, color);
    //DrawTextStyled(font, "Text, %^0FText, %^F2Text, %^22Text,", Vector2{200, 200}, 20, 1, WHITE);  
    //RendererInstance.DrawText(font, text, Vector2 position, float fontSize, float spacing, Color color);

    return 0;
}

static const luaL_Reg ApiRenderer[] = {
    {"DrawText", Renderer_DrawText},
    
    {NULL, NULL},
};

int luaopenRenderer(lua_State* L)
{
    luaL_registerwithclosure(L, "Renderer", ApiRenderer, 1);
    return 1;
}
