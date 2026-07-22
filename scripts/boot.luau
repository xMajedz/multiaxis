--local Console = require "Ui/Console" -- load console first so i can get errors logged
--runscript("redpixel.luau")
--runscript("von.luau")

local ui = require "@uielement"

if true then
local viewElement = ui.new{
    rec = {10, ui.WIN_H - 25, ui.WIN_W - 20, 20 },

    bgColor = ui.UICOLORBLANK,
    hoverColor = ui.UICOLORBLANK,
    pressedColor = ui.UICOLORBLUE,
    
    borderColor = ui.UICOLORWHITE,
    borderHoverColor = ui.UICOLORBLUE,
    
    interactive = true,
}

viewElement:hide()

--viewElement:makeTextfield(textfieldString, textfieldSettings)

local text = "textfield"

function viewElement.customDisplay()
    viewElement:uiText(text, 5, 0, (viewElement.hoverState and ui.UICOLORBLUE) or ui.UICOLORWHITE)
end

function viewElement.enterAction()
    if viewElement.displayed then viewElement:hide() else viewElement:show() end
end

--function viewElement.keyPressed(key) end

function Api.OnKeyPressed.textfield(key)
    -- TODO: safely convert key to char in c api
    --if key ~= 257 then text = text .. string.char(key) end
end

ui.mouseHooks()
ui.keyboardHooks()
ui.renderHooks()
end

local viewElement = ui.new {rec = {(ui.WIN_W - 100)/2, (ui.WIN_H - 100)/2, 100, 100}, shapeType = ui.SHAPE_ROUNDED}

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
