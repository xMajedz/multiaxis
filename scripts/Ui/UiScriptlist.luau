local UiScriptlist = {}

local show = false

local list = {}

local loaded = "init;Api;"

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function UiScriptlist.Toggle()
    show = show == false

    if show then
	    list = RAYLIB.LoadDirectoryFiles("scripts")
	end
end

function Api.Draw.UiScriptlist()
    if show then
		if RAYGUI.GuiWindowBox(10, 40, (screenWidth/4) - 20, screenHeight - 100, "Scripts") then
			show = false
		end

		for i = 1, #list do
			local script = list[i]:gsub("scripts[/\\]", "")
			RAYGUI.GuiLabel(20, 10 + 30 * (i + 1), 400, 20, script)

			if script:match(".+%.luau") and not loaded:match(script:gsub("%.luau", "")) then
				if (RAYGUI.GuiButton(350, 10 + 30 * (i + 1), 25, 25, `#{RAYGUI["ICON_PLAYER_PLAY"]}#`)) then
					script = script:gsub("%..+", "")

					Api.loadscript(script)

                    loaded = loaded .. script .. ";"

					show = false
				end
			end
		end
	end
end

return UiScriptlist