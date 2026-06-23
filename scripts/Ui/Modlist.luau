local Modlist = { Menu = { showUi = true } }

local ms = require "modstuff"

local show = false

local list = {}

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function Modlist.Toggle()
    show = show == false

	if show then
	    list = RAYLIB.LoadDirectoryFiles("mods");
	end
end

function Api.Draw.UiModlist()
if Modlist.Menu.showUi then
   if show then
        if RAYGUI.GuiWindowBox(10, 40, (screenWidth/4) - 20, screenHeight - 100, "Mods") then
		    show = false
	    end

	    local i, j = 1, 1
	
	    while i <= #list do
		    local mod = list[i]:gsub("mods[/\\]", "")

		    if not mod:match(".+%.txt") then
			    RAYGUI.GuiLabel(20, 10 + 30 * (j + 1), 400, 20, mod)

			    if RAYGUI.GuiButton(350, 10 + 30 * (j + 1), 25, 25, `#{RAYGUI["ICON_PLAYER_PLAY"]}#`) then
				    if mod:match("^[%.%w%-_]+%.luau") then
					    --Api.Reset()
					    --loadmod(mod:match("^[%w%-_]+"))
					    --Game.Reset()
					    --Game.ImportMod()
					    --Game.NewGame()
				    end
				
				    if mod:match("^[%.%w%-_]+%.tbm") then
					    --Api.Reset()
					    --ms.portTBM(mod:match("^[%w%-_]+"))
					    loadmod(`./mods/{mod}`)
					    --Game.Reset()
					    --Game.ImportMod()
					    --Game.NewGame()
				    end
	
				    show = false
			    end
			    j += 1
		    end
		    i += 1
	    end
   end
end
end

return Modlist
