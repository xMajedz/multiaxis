--local Console = require "Ui/Console" -- load console first so i can get errors logged
--runscript("redpixel.luau")
--runscript("von.luau")

local ui = require "@uielement"

do
local viewElement = ui.new{
   rec = {10, ui.WIN_H - 25, ui.WIN_W - 20, 20 },
   bgColor = ui.UICOLORRED,
   borderColor = ui.UICOLORBLUE,
   hoverColor = ui.UICOLORBLUE,
   pressedColor = ui.UICOLORBLUE,
   interactive = true,
}

ui.mouseHooks()
ui.keyboardHooks()
ui.renderHooks()
end
-- uielement.new() no arguments default constructor behaviour and throw an error
-- uielement.new{} table argument default constructor behaviour
-- uielement:new() self(table) argument default constructor behaviour
--uielement.new()
--local viewElement = uielement.new {size = {uielement.WIN_W, uielement.WIN_H}, uiColor = uielement["UICOLORBLUE"]}
--viewElement:addAdaptedText("addAdaptedText")


--function viewElement.customDisplay() end
--function viewElement.customDisplayBefore() end

--viewElement:addCustomDisplay(function() end)
--viewElement:addCustomDisplay(false, function() end)
--viewElement:addCustomDisplay(false, false, function() end)

--[[
for i, message in require("intro") do -- insert the intro message into the chat
   Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

Console.Menu = require "Ui/Menu" -- load the menu
Console.Menu.Console = Console
]]
