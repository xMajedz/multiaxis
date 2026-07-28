--local Console = require "Ui/Console" -- load console first so i can get errors logged

local ui = require "@uielement"

ui.mouseHooks()
ui.keyboardHooks()
ui.renderHooks()

local viewElement = ui.new{size = {ui.WIN_W, ui.WIN_H}, bgColor = ui.UICOLORBLANK}

local ui3d = require "@uielement3d"

--ui3d.renderHooks()
--[[
local obj = ui3d.new{bgColor = ui.UICOLORGREEN}

function Api.OnRenderGame.boot(pass)
    obj:display(pass)
end
]]
--runscript("redpixel.luau")
--runscript("von.luau")

--[[
function makeTextfield(o, textfieldString)
    local viewElement = ui.new{
        rec = {10, ui.WIN_H - 25, ui.WIN_W - 20, 20 },

        bgColor = ui.UICOLORBLANK,
        hoverColor = ui.UICOLORBLANK,
        pressedColor = ui.UICOLORBLUE,
    
        borderColor = ui.UICOLORWHITE,
        borderHoverColor = ui.UICOLORBLUE,
     
        interactive = true,
	
	keyboard = true,
    }

    viewElement:hide()

    function viewElement.customDisplay()
        viewElement:uiText(textfieldString, 5, 0, (viewElement.hoverState and ui.UICOLORBLUE) or ui.UICOLORWHITE)
    end
    
    function viewElement.enterAction()
        if viewElement.displayed then viewElement:hide() else viewElement:show() end
    end
    
    function viewElement.keyPressed(key)
       --log(key)
    end
end

makeTextfield(nil, "textfield")
]]
function makeMenu()
    local viewElement = ui.new{rec = {0, 10, ui.WIN_W, 25}, bgColor = ui.UICOLORRED}

    local offset = 10
    local button_size = 100
    local gape = (ui.WIN_W - 2 * button_size + 10)/3
  
    local rulesButton = viewElement:addChild{
        rec = {offset, 10, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
        interactive = true,
    }
    
    rulesButton:addAdaptedText("Rules")
    
    offset += gape 
    local modlistButton = viewElement:addChild{
        rec = {offset, 10, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
        interactive = true,
    }
    
    modlistButton:addAdaptedText("Mods")

    offset += gape
    local replaylistButton = viewElement:addChild{
        rec = {offset, 10, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	interactive = true,
    }
    
    replaylistButton:addAdaptedText("Replays")

    offset += gape
    local scriptlistButton = viewElement:addChild{
        rec = {offset, 10, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	interactive = true,
    }
    
    scriptlistButton:addAdaptedText("Scripts")

    local settingsButton = viewElement:addChild{
        rec = {ui.WIN_W - 70, 10, 25, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
        interactive = true,
    }

    settingsButton:addAdaptedText("1")

    local closeButton = viewElement:addChild{
        rec = {ui.WIN_W - 35, 10, 25, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
        interactive = true,
    }

    closeButton:addAdaptedText("2")

    function closeButton.mouseButtonPressed()
        log("viewElement:kill()")
        viewElement:kill()
        viewElement = nil
	Game.Quit()
    end
end

makeMenu()

--[[
for i, message in require("intro") do -- insert the intro message into the chat
   Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

Console.Menu = require "Ui/Menu" -- load the menu
Console.Menu.Console = Console
]]
