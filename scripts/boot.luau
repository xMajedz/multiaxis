local Ray = require "@Raylib"
local Gui = require "@Raygui"
local Math = require "@Raymath"

sandbox = "1"

Gui.GuiSetStyle(0, 16, 30)

local fg_color = Gui.ColorToInt(Ray["RED"])

function Api.RenderForeground.boot()
    Ray.DrawRectangle(0, 0, 200, 20, Ray["BLUE"])
    Gui.GuiSetStyle(0, 2, fg_color)
    Gui.GuiLabel(0, 0, 200, 20, "RayGui")
end

local bg_color = Gui.ColorToInt(Ray["BLUE"])

function Api.RenderBackground.boot()
    Ray.DrawRectangle(0, 20, 200, 20, Ray["RED"])
    Gui.GuiSetStyle(0, 2, bg_color)
    Gui.GuiLabel(0, 20, 200, 20, "RayGui")
end

--[[
local Console = require "Ui/Console" -- load console first so i can get errors logged

for i, message in require("intro") do -- insert the intro message into the chat
   Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

Console.Menu = require "Ui/Menu" -- load the menu
Console.Menu.Console = Console
]]
