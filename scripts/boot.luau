local UIElement = require "@UIElement"

local viewElement = UIElement.new({ pos = {10, 10}, size = {90, 90}, bgColor = UIElement["UICOLORRED"] })

--viewElement:hide()
--[[
function Api.RenderForeground.UIElement()
    viewElement:display()
end
]]

function Api.RenderForeground.UIElement()
    UIElement.drawVisuals()
end

local Console = require "Ui/Console" -- load console first so i can get errors logged

--[[
for i, message in require("intro") do -- insert the intro message into the chat
   Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

Console.Menu = require "Ui/Menu" -- load the menu
Console.Menu.Console = Console
]]
