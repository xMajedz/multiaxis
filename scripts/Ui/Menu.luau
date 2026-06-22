local Settings = require "Ui/Settings"

local Gamerules = require "Ui/Gamerules"

local Modlist = require "Ui/Modlist"

local Replaylist = require "Ui/Replaylist"
local Replayslider = require "Ui/Replayslider"
local Replaysave = require "Ui/Replaysave"

local Scriptlist = require "Ui/Scriptlist"

local Menu = { showUi = true }

Settings.Menu = Menu

Gamerules.Menu = Menu

Modlist.Menu = Menu

Replaylist.Menu = Menu
Replayslider.Menu = Menu
Replaysave.Menu = Menu

Scriptlist.Menu = Menu

local showMenu = false

local mod, rules, gravity, frame, reaction_count

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

Api.SetHotKey(RAYLIB["KEY_TAB"], function() Menu.showUi = Menu.showUi == false end)
Api.SetHotKey(RAYLIB["KEY_ESCAPE"], function() showMenu = showMenu == false end)
Api.SetHotKey(RAYLIB["KEY_R"], function() Game.EnterMode(Game["MODE_REPLAY"], false) end)
Api.SetHotKey(RAYLIB["KEY_P"], function() Game.TogglePause() end)
Api.SetHotKey(RAYLIB["KEY_G"], function() Game.ToggleGhosts() end)

--Api.SetHotKey(Game["MODE_REPLAY"], RAYLIB["KEY_SPACE"], function() Game.EnterMode(Game["MODE_FREEPLAY"], true) end)
--Api.SetHotKey(Game["MODE_REPLAY"], RAYLIB["KEY_E"], function() Game.EnterMode(Game["MODE_REPLAY_EDIT"], false) end)

--[[
Api.SetHotKey(Game["MODE_FREEPLAY"], RAYLIB["KEY_SPACE"], function()
   if Game.IsFreeze() then
       if rules then
		   local step_size = (RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) and 1) or rules.turnframes
		   Game.Step(step_size)
	   end
   end
end)
]]

RAYGUI.GuiSetStyle(0, 16, 20)

function Api.NewGame.Menu()
    mod, rules = Game.GetMod(), Game.GetGamerules()
end

function Api.Update.Menu(dt)
	frame = Game.GetGameFrame()

	if rules then
		reaction_count = math.round(rules.reaction_time - Game.GetReactionCount())
	end

	if Game.IsFreeze() and not Game.IsMode(Game["MODE_REPLAY"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_SPACE"]) then
	    if rules then
		   local step_size = (RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) and 1) or rules.turnframes
		   Game.Step(step_size)
		end
	end

	if Game.IsMode(Game["MODE_REPLAY"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_SPACE"]) then
		Game.EnterMode(Game["MODE_FREEPLAY"], true)
	end

	if Game.IsMode(Game["MODE_REPLAY"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_E"]) then
		Game.EnterMode(Game["MODE_REPLAY_EDIT"], false)
	end

	if Game.IsFreeze() and Game.IsMode(Game["MODE_FREEPLAY"]) and Game.IsSelectedJointValid() then
		if not RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_CONTROL"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_Z"]) then
			Game.Refreeze()

			if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				Game.ToggleSelectedJointActiveStateAlt()
			else
				Game.ToggleSelectedJointActiveState()
			end
		end

		if RAYLIB.IsKeyPressed(RAYLIB["KEY_X"]) then
			Game.Refreeze()

			if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				Game.ToggleSelectedJointPassiveStateAlt()
			else
				Game.ToggleSelectedJointPassiveState()
			end
		end

		if RAYLIB.IsMouseButtonPressed(RAYLIB["MOUSE_BUTTON_LEFT"]) then
			Game.Refreeze()

			if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				Game.CycleSelectedJointStateAlt()
			else
				Game.CycleSelectedJointState()
			end
		end
		
		local WheelMove = RAYLIB.GetMouseWheelMove()

        if WheelMove > 0 then
		    Game.Refreeze()

            if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				Game.CycleSelectedJointStateAlt()
			else
				Game.CycleSelectedJointState()
			end
		elseif WheelMove < 0 then
		    Game.Refreeze()
			
		    if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				Game.ReverseCycleSelectedJointStateAlt()
			else
				Game.ReverseCycleSelectedJointState()
			end
		end
	end

	if Game.IsFreeze() and Game.IsMode(Game["MODE_FREEPLAY"]) and Game.IsSelectedPlayerValid() then
		if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_CONTROL"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_Z"]) then
			Game.Refreeze()

			Game.UndoSelectedPlayerMove()
		end

		if RAYLIB.IsKeyPressed(RAYLIB["KEY_C"]) then
			Game.Refreeze()

			Game.ToggleSelectedPlayerPassiveStatesAlt()
			Game.ToggleSelectedPlayerPassiveStates()
		end

		if RAYLIB.IsKeyPressed(RAYLIB["KEY_V"]) then
			Game.Refreeze()

			Game.ToggleSelectedPlayerBodyStates()
		end
	end
end

function Api.Draw.Menu()
if Menu.showUi then
	if showMenu then
		local offset = 10
		local button_size = 100
		local gape = (screenWidth - 2 * button_size + 10)/3
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Rules") then
		    Gamerules.Toggle()
		end
		offset = offset + gape 
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Mods") then
		    Modlist.Toggle()
		end
		offset = offset + gape
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Replays") then
		    Replaylist.Toggle()
		end
		offset = offset + gape
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Scripts") then
		    Scriptlist.Toggle()
		end

		if RAYGUI.GuiButton(screenWidth - 70, 10, 25, 25, `#{RAYGUI["ICON_GEAR_BIG"]}#`) then
		    Settings.Toggle()
		end
		if RAYGUI.GuiButton(screenWidth - 35, 10, 25, 25, `#{RAYGUI["ICON_CROSS"]}#`) then
			Game.Quit()
		end
	else
		if rules then
			local offset = 10
			RAYGUI.GuiLabel(10, offset, screenWidth - 10, 10, `{frame}`)
			offset = offset + 20
			if rules.reaction_time > 0 then
				RAYGUI.GuiLabel(10, offset, screenWidth - 10, 10, `{reaction_count}`)
			end
		end
	end
end
end

function Api.FileDropped.Menu(files) for i, file in files do log(i, file) end end

return Menu
