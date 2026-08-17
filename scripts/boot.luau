local ui = require "@uielement"
ui.mouseHooks()
ui.keyboardHooks()
ui.renderHooks()

--local viewElement = ui.new{size = {ui.WIN_W, ui.WIN_H}, bgColor = ui.UICOLORBLANK}

local ui3d = require "@uielement3d"
ui3d.renderHooks()

--local obj0 = ui3d.new{size = {0.50, 0.50, 0.50}, bgColor = ui.UICOLORGREEN}
--local obj1 = ui3d.new{size = {5.00, 0.25, 0.25}, bgColor = ui.UICOLORGREEN}
--local obj2 = ui3d.new{size = {0.25, 5.00, 0.25}, bgColor = ui.UICOLORGREEN}
--local obj3 = ui3d.new{size = {0.25, 0.25, 5.00}, bgColor = ui.UICOLORGREEN}

--runscript("redpixel.luau")
--runscript("von.luau")

local console = require "ui/console"

local consoleObject = console.makeTextfield("textfield")

local menu = require "ui/menu"

local menuObject = menu.makeMenu()

--[[
for i, message in require("intro") do -- insert the intro message into the chat
   Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

Console.Menu = require "Ui/Menu" -- load the menu
Console.Menu.Console = Console
]]
