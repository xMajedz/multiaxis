local ui = require "@uielement"

local menu = {}

local placeholderFunc = function() end

local spawnModlist = function()
    --Game.LoadModFile("box.tbm")
    runscript("LoadModText.luau")
    Game.NewGame()
end
    
local spawnScriptlist = function()
    runscript("DrawText.luau")
end

local menuButtons = {
    { text = "Rules",   func = placeholderFunc},
    { text = "Mods",    func = spawnModlist},
    { text = "Replays", func = placeholderFunc},
    { text = "Scripts", func = spawnScriptlist},
}

local count = 0

local countUp = function()
    count += 1
    log(count)
end

local ctrlButtons = {
    { text = "1", func = Game.Quit},
    { text = "2", func = countUp},
}

function menu.makeMenu()
    local viewElement = ui.new{rec = {10, 10, ui.WIN_W - 20, 25}, bgColor = ui.UICOLORRED}

    local offset = 0
    local button_size = 100
    local gape = (ui.WIN_W - 2 * button_size + 10)/3

    for i, data in menuButtons do
        local btn = viewElement:addChild{
            rec = {offset, 0, button_size, 25},
            bgColor = ui.UICOLORRED,
            hoverColor = ui.UICOLORBLUE,
	    pressedColor = ui.UICOLORRED,
            interactive = true,
        }

	btn:addAdaptedText(data.text)

	btn.mouseButtonPressed = data.func

	offset += gape
    end

    for i, data in ctrlButtons do        
        local btn = viewElement:addChild{
	    rec = {-25 + (-35) * (i - 1), 0, 25, 25},
            bgColor = ui.UICOLORRED,
            hoverColor = ui.UICOLORBLUE,
	    pressedColor = ui.UICOLORRED,
            interactive = true,
        }

        btn:addAdaptedText(data.text)

        btn.mouseButtonPressed = data.func
    end
end

return menu
