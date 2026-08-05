local ui = require "@uielement"
ui.mouseHooks()
ui.keyboardHooks()
ui.renderHooks()

--local viewElement = ui.new{size = {ui.WIN_W, ui.WIN_H}, bgColor = ui.UICOLORBLANK}

local ui3d = require "@uielement3d"
ui3d.renderHooks()

local obj0 = ui3d.new{size = {0.50, 0.50, 0.50}, bgColor = ui.UICOLORGREEN}
local obj1 = ui3d.new{size = {5.00, 0.25, 0.25}, bgColor = ui.UICOLORGREEN}
local obj2 = ui3d.new{size = {0.25, 5.00, 0.25}, bgColor = ui.UICOLORGREEN}
local obj3 = ui3d.new{size = {0.25, 0.25, 5.00}, bgColor = ui.UICOLORGREEN}

--local Console = require "Ui/Console" -- load console first so i can get errors logged
--runscript("redpixel.luau")
--runscript("von.luau")

function makeTextfield(textfieldString)
    local textfieldBuffer = buffer.create(256)

    local textfieldIndex = #textfieldString

    buffer.writestring(textfieldBuffer, 0, textfieldString)

    local viewElement = ui.new{
        rec = {10, ui.WIN_H - 25, ui.WIN_W - 20, 20},

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
        local color = (viewElement.hoverState and ui.UICOLORBLUE) or ui.UICOLORWHITE
        local text = buffer.readstring(textfieldBuffer, 0, textfieldIndex)
	viewElement:uiText(text .. "_", 5, 0, nil, 0, nil, 20, color)
    end
    
    function viewElement.enterAction()
        if viewElement.displayed then
	    if textfieldIndex > 0 then
	      log(buffer.readstring(textfieldBuffer, 0, textfieldIndex))
	      textfieldIndex = 0
	    end
	    viewElement:hide()
	else
	    viewElement:show()
	end
    end
    
    function viewElement.keyPressed(key, keyCode)
        if keyCode == 259 and textfieldIndex > 0 then
	    textfieldIndex -= 1
	end
        if key > 0 and textfieldIndex < 256 and viewElement.displayed then
	    buffer.writeu8(textfieldBuffer, textfieldIndex, key)
            textfieldIndex += 1
        end
    end

    return viewElement
end

local viewElement = makeTextfield("textfield")

function makeMenu()
    local viewElement = ui.new{rec = {10, 10, ui.WIN_W - 20, 25}, bgColor = ui.UICOLORRED}

    local offset = 0
    local button_size = 100
    local gape = (ui.WIN_W - 2 * button_size + 10)/3
    
    local rulesButton = viewElement:addChild{
        rec = {offset, 0, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	pressedColor = ui.UICOLORRED,
        interactive = true,
    }
    
    rulesButton:addAdaptedText("Rules")
    
    offset += gape 
    local modlistButton = viewElement:addChild{
        rec = {offset, 0, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	pressedColor = ui.UICOLORRED,
        interactive = true,
    }
    
    modlistButton:addAdaptedText("Mods")

    function modlistButton.mouseButtonPressed()
       --Game.LoadModFile("box.tbm")
        runscript("LoadModText.luau")
        Game.NewGame()
    end
    
    offset += gape
    local replaylistButton = viewElement:addChild{
        rec = {offset, 0, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	pressedColor = ui.UICOLORRED,
	interactive = true,
    }
    
    replaylistButton:addAdaptedText("Replays")

    offset += gape
    local scriptlistButton = viewElement:addChild{
        rec = {offset, 0, button_size, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	pressedColor = ui.UICOLORRED,
	interactive = true,
    }
    
    scriptlistButton:addAdaptedText("Scripts")

    local settingsButton = viewElement:addChild{
        rec = {-60, 0, 25, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	pressedColor = ui.UICOLORRED,
        interactive = true,
    }

    settingsButton:addAdaptedText("1")

    local closeButton = viewElement:addChild{
        rec = {-25, 0, 25, 25},
        bgColor = ui.UICOLORRED,
        hoverColor = ui.UICOLORBLUE,
	pressedColor = ui.UICOLORRED,
        interactive = true,
    }

    closeButton:addAdaptedText("2")

    closeButton.mouseButtonPressed = Game.Quit
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
