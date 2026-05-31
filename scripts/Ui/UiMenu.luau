local require = Api.require

local UiSettings = require "Ui/UiSettings"

local UiGamerules = require "Ui/UiGamerules"

local UiModlist = require "Ui/UiModlist"

local UiReplaylist = require "Ui/UiReplaylist"
local UiReplayslider = require "Ui/UiReplayslider"
local UiReplaysave = require "Ui/UiReplaysave"

local UiScriptlist = require "Ui/UiScriptlist"

local showUi = true
local showMenu = false

local mod, o_count, p_count, p_details, frame, contact_count, reaction_count, rules, gravity

RAYGUI.GuiSetStyle(0, 16, 20)

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function Api.NewGame.UiMenu()
    mod = Game.GetMod()
    rules = Game.GetGamerules()

	o_count = Game.GetObjectCount()
	p_count = Game.GetPlayerCount()
	p_details = ""
	for pID = 0, p_count - 1 do
		local b_count = Game.GetPlayerBodyCount(pID)
		local j_count = Game.GetPlayerJointCount(pID)
		p_details = p_details .. "p" .. (pID + 1) .. ": "
		p_details = p_details .. "b:" .. b_count .. " "
		p_details = p_details .. "j:" .. j_count .. " "
	end
end

function Api.Update.UiMenu(dt)
	frame = Game.GetGameFrame()
	contact_count = Game.GetContactCount()

	if rules then
		reaction_count = math.round(rules.reaction_time - Game.GetReactionCount())
	end

	if RAYLIB.IsKeyPressed(RAYLIB["KEY_TAB"]) then
		showUi = showUi == false
	end
		
if Net then
else	
	if RAYLIB.IsKeyPressed(RAYLIB["KEY_R"]) then
		Game.EnterMode(Game["MODE_REPLAY"], false)
	end

	if RAYLIB.IsKeyPressed(RAYLIB["KEY_P"]) then
		Game.TogglePause()
	end

	if RAYLIB.IsKeyPressed(RAYLIB["KEY_G"]) then
		Game.ToggleGhosts()
	end

	if RAYLIB.IsKeyPressed(RAYLIB["KEY_ESCAPE"]) then
		showMenu = showMenu == false
	end

	if Game.IsFreeze() and not Game.IsMode(Game["MODE_REPLAY"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_SPACE"]) then
		local step_size = (RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) and 1) or rules.turnframes
		Game.Step(step_size)
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
end

function Api.Draw.UiMenu()
if showUi then
	if showMenu then
		local offset = 10
		local button_size = 100
		local gape = (screenWidth - 2 * button_size + 10)/3
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Rules") then
		    UiGamerules.Toggle()
		end
		offset = offset + gape 
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Mods") then
		    UiModlist.Toggle()
		end
		offset = offset + gape
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Replays") then
		    UiReplaylist.Toggle()
		end
		offset = offset + gape
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Scripts") then
		    UiScriptlist.Toggle()
		end

		if RAYGUI.GuiButton(screenWidth - 70, 10, 25, 25, `#{RAYGUI["ICON_GEAR_BIG"]}#`) then
		    UiSettings.Toggle()
		end
		if RAYGUI.GuiButton(screenWidth - 35, 10, 25, 25, `#{RAYGUI["ICON_CROSS"]}#`) then
			Game.Quit()
		end
	else
		if rules then
			local offset = 10
			RAYGUI.GuiLabel(10, offset, screenWidth-10, 10, `{frame}`)
			offset = offset + 20
			if rules.reaction_time > 0 then
				RAYGUI.GuiLabel(10, offset, screenWidth-10, 10, `{reaction_count}`)
			end
		end
	end
end
end
