local UiSettings = {}

local show = false

local darkMode = false

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

local windowWidth = { buffer = buffer.create(256), edit = false }
local windowHeight = { buffer = buffer.create(256), edit = false }

buffer.writestring(windowWidth.buffer, 0, tostring(screenWidth))
buffer.writestring(windowHeight.buffer, 0, tostring(screenHeight))

function UiSettings.Toggle()
    show = show == false
end

function Api.Draw.UiSettings()
    if show then
		if RAYGUI.GuiWindowBox(10, 40, (screenWidth/4) - 10, screenHeight - 100, "Settings") then
			show = false
		end

		RAYGUI.GuiLabel(20, 70, 400, 20, "Resolution")

		if RAYGUI.GuiTextBox2(120, 70, 90, 20, windowWidth.buffer, windowWidth.edit) then
			windowWidth.edit = windowWidth.edit == false
		end

		if RAYGUI.GuiTextBox2(215, 70, 90, 20, windowHeight.buffer, windowHeight.edit) then
			windowHeight.edit = windowHeight.edit == false
		end

		if (RAYGUI.GuiButton(310, 70, 90, 20, "Apply")) then
			local width = buffer.readstring(windowWidth.buffer, 0, buffer.len(windowWidth.buffer))
			local height = buffer.readstring(windowHeight.buffer, 0, buffer.len(windowHeight.buffer))
			screenWidth, screenHeight = width, height
			RAYLIB.SetWindowSize(width, height)
		end

		RAYGUI.GuiLabel(20, 95, 200, 20, "Fullscreen Mode")

		if (RAYGUI.GuiButton(310, 95, 90, 20, (RAYLIB.IsWindowFullscreen() and "Disable") or "Enable")) then
			RAYLIB.ToggleFullscreen()
		end

		RAYGUI.GuiLabel(20, 120, 200, 20, "Dark Mode")

		if (RAYGUI.GuiButton(310, 120, 90, 20, (darkMode and "Disable") or "Enable")) then
			darkMode = darkMode == false
			if darkMode then
				Game.SetBackgroundColor(0, 0, 0, 255)
			else
				Game.SetBackgroundColor(255, 255, 255, 255)
			end
		end
		
	    RAYGUI.GuiLabel(20, 145, 200, 20, "Self Play")

        if (RAYGUI.GuiButton(310, 145, 90, 20, (Game.IsMode(Game["MODE_SELFPLAY"]) and "Disable") or "Enable")) then
		    if not Game.IsMode(Game["MODE_SELFPLAY"]) then
			    Game.SetMode(Game["MODE_SELFPLAY"])
			else
			    Game.SetMode(Game["MODE_FREEPLAY"])
			end
		end

        RAYGUI.GuiLabel(20, 170, 400, 20, "Ghost Cache")

        if (RAYGUI.GuiButton(310, 170, 90, 20, (Game.GhostCacheEnabled() and "Disable") or "Enable")) then
		    Game.ToggleGhostCache()
		end
		
		RAYGUI.GuiLabel(20, 195, 400, 20, "Show Next Turn Ghost")

        if (RAYGUI.GuiButton(310, 195, 90, 20, (Game.TurnFrameGhostEnabled() and "Disable") or "Enable")) then
		    Game.ToggleTurnFrameGhost()
		end
		
		RAYGUI.GuiLabel(20, 220, 400, 20, "Replay Cache")

        if (RAYGUI.GuiButton(310, 220, 90, 20, (Game.ReplayCacheEnabled() and "Disable") or "Enable")) then
		    Game.ToggleReplayCache()
		end
	end
end

return UiSettings