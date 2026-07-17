--local Console = require "Ui/Console" -- load console first so i can get errors logged
--runscript("redpixel.luau")
--runscript("von.luau")

local uielement = require "@uielement"
local clock, viewElement = uielement.clock, uielement.new {size = {uielement.WIN_W, uielement.WIN_H}, bgColor = uielement["UICOLORWHITE"]}

--function viewElement.customDisplay() end
--function viewElement.customDisplayBefore() end
--viewElement:addCustomDisplay(function() end)
--viewElement:addCustomDisplay(nil, function() end)
--viewElement:addCustomDisplay(false, function() end)
--viewElement:addCustomDisplay(false, function() end, function() end)
--viewElement:addCustomDisplay(false, nil, function() end)

function customDisplayFunc(message) return function() log(message) end end
viewElement:addCustomDisplay(false, customDisplayFunc("customDisplay"), customDisplayFunc("cutsomDisplayBefore"))

--[[
for i, message in require("intro") do -- insert the intro message into the chat
   Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

Console.Menu = require "Ui/Menu" -- load the menu
Console.Menu.Console = Console
]]
