local require = Api["require"]
local ms = require "modstuff"

local UiGamerules = require "Ui/UiGamerules"

local darkMode = false

local showGui = true
local showMenu = false

local showMods = false

local showReplays = false
local showReplaysSaving = false

local showScripts = false
local showSettings = false

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

local Data = {}

Data["ReplayName_GuiTextBox"] = Api.CreateData(1024)

Data["Settings_WindowWidth_GuiTextBox"] = Api.CreateData(1024)
Data["Settings_WindowHeight_GuiTextBox"] = Api.CreateData(1024)

Api.StringToData(tostring(screenWidth), Data["Settings_WindowWidth_GuiTextBox"])
Api.StringToData(tostring(screenHeight) ,Data["Settings_WindowHeight_GuiTextBox"])

local editValue = {}

editValue["ReplayName_GuiTextBox"] = false

editValue["Settings_WindowWidth_GuiTextBox"] = false
editValue["Settings_WindowHeight_GuiTextBox"] = false

local replay_slider_value = Api.CreateData(4)

local replay_list, mod_list, script_list
local mod, o_count, p_count, p_details, frame, contact_count, reaction_count, rules, gravity, console_message

local loaded_scripts = {}
loaded_scripts["UIMenu"] = 1
loaded_scripts["UIConsole"] = 1
loaded_scripts["intro"] = 1
loaded_scripts["init"] = 1
loaded_scripts["Api"] = 1

RAYGUI.GuiSetStyle(0, 16, 20)

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
		showGui = showGui == false
	end

	if RAYLIB.IsKeyPressed(RAYLIB["KEY_F"]) then
		showReplaysSaving = true
		Replay.Export("tempreplayfile.bin")
	end
	
	if Game.IsPause() and Game.IsMode(Game["MODE_REPLAY"]) and Game.ReplayCacheEnabled() then
	    local value = Api.NumberFromData(replay_slider_value)
		Game.SetGameFrame(value * Replay.GetMaxFrame())
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

			if loaded_scripts["variable_joint_velocity"] == nil then
				if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
					Game.ToggleSelectedJointActiveStateAlt()
				else
					Game.ToggleSelectedJointActiveState()
				end
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
if showGui then
	if showMenu then
		local offset = 10
		local button_size = 100
		local gape = (screenWidth - 2 * button_size + 10)/3
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Gamerules") then
		   UiGamerules.Toggle()
		end
		offset = offset + gape 
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Mods") then
			showMods = showMods == false
			if showMods then
				mod_list = RAYLIB.LoadDirectoryFiles("mods");
			end
		end
		offset = offset + gape
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Replays") then
			showReplays = showReplays == false
			if showReplays then
				replay_list = RAYLIB.LoadDirectoryFiles("replays")
			end
		end
		offset = offset + gape
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Scripts") then
			showScripts = showScripts == false
			if showScripts then
				script_list = RAYLIB.LoadDirectoryFiles("scripts")
			end
		end

		if RAYGUI.GuiButton(screenWidth - 70, 10, 25, 25, `#{RAYGUI["ICON_GEAR_BIG"]}#`) then
			showSettings = showSettings == false
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

	if showMods then 
		if RAYGUI.GuiWindowBox(10, 40, 400, 400, "Mods") then
			showMods = false
		end

		local i = 1
		local j = 1
		while i <= #mod_list do
			local mod = mod_list[i]:gsub("mods[/\\]", "")
			if not mod:match(".+%.txt") then
				RAYGUI.GuiLabel(20, 10 + 30 * (j + 1), 400, 20, mod)

				if RAYGUI.GuiButton(350, 10 + 30 * (j + 1), 25, 25, `#{RAYGUI["ICON_PLAYER_PLAY"]}#`) then
					if mod:match("^[%.%w%-_]+%.luau") then
						Api.Reset()
						Api.loadmod(mod:match("^[%w%-_]+"))
						Game.Reset()
						Game.ImportMod()
						Game.NewGame()
					end
					if mod:match("^[%.%w%-_]+%.tbm") then
						Api.Reset()
						ms.loadmodTBM2(mod:match("^[%w%-_]+"))
						Game.Reset()
						Game.ImportMod()
						Game.NewGame()
					end
	
					showMods = false
				end

				j = j + 1
			end

			i = i + 1
		end
	end

	if showReplays then
		if RAYGUI.GuiWindowBox(10, 40, 400, 400, "Replays") then
			showReplays = false
		end

		for i = 1, #replay_list do
			local replay = replay_list[i]:gsub("replays[/\\]", "")
			RAYGUI.GuiLabel(20, 10 + 30 * (i + 1), 400, 20, replay)

			if not replay:match(".+%.txt") then
				if RAYGUI.GuiButton(350, 10 + 30 * (i + 1), 25, 25, `#{RAYGUI["ICON_PLAYER_PLAY"]}#`) then
					if replay:match("^[%.%w%-_]+%.rpl") then
						Replay.Import(replay)
						local mod = Replay.GetMod()
						if mod == mod_name then
							if mod:match("^CONVERTED_.+") then
								Api.Reset()
								local mod_name = mod:gsub("CONVERTED_", "")
								ms.loadmodTBM2(mod_name)
								Game.ImportMod()
								Game.NewGame()
							end
						end
						Game.EnterMode(Game["MODE_REPLAY"], true)
					end
	
					showReplays = false
				end
			end
		end
	end

	if showScripts then
		if RAYGUI.GuiWindowBox(10, 40, 400, 400, "Scripts") then
			showScripts = false
		end

		for i = 1, #script_list do
			local script = script_list[i]:gsub("scripts[/\\]", "")
			RAYGUI.GuiLabel(20, 10 + 30 * (i + 1), 400, 20, script)

			if script:match(".+%.luau") and not loaded_scripts[script:gsub("%.luau", "")] then
				if (RAYGUI.GuiButton(350, 10 + 30 * (i + 1), 25, 25, `#{RAYGUI["ICON_PLAYER_PLAY"]}#`)) then
					script = script:gsub("%..+", "")

					Api.loadscript(script)

					loaded_scripts[script] = 1

					showScripts = false
				end
			end
		end
	end

	if showSettings then
		if RAYGUI.GuiWindowBox(10, 40, (screenWidth/4) - 10, screenHeight - 100, "Settings") then
			showSettings = false
		end

		RAYGUI.GuiLabel(20, 70, 400, 20, "Resolution")

		if RAYGUI.GuiTextBox(120, 70, 90, 20, Data["Settings_WindowWidth_GuiTextBox"], 1024, editValue["Settings_WindowWidth_GuiTextBox"]) then
			editValue["Settings_WindowWidth_GuiTextBox"] = editValue["Settings_WindowWidth_GuiTextBox"] == false
		end

		if RAYGUI.GuiTextBox(215, 70, 90, 20, Data["Settings_WindowHeight_GuiTextBox"], 1024, editValue["Settings_WindowHeight_GuiTextBox"]) then
			editValue["Settings_WindowHeight_GuiTextBox"] = editValue["Settings_WindowHeight_GuiTextBox"] == false
		end

		if (RAYGUI.GuiButton(310, 70, 90, 20, "Apply")) then
			local width = Api.StringFromData(Data["Settings_WindowWidth_GuiTextBox"])
			local height = Api.StringFromData(Data["Settings_WindowHeight_GuiTextBox"])
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

	local offset = screenHeight-30

	if Game.IsMode(Game["MODE_REPLAY"])then
		Api.NumberToData(frame/Replay.GetMaxFrame(), replay_slider_value)
		RAYGUI.GuiSliderBar(10, screenHeight - 15, screenWidth - 20, 10, "", "", replay_slider_value, 0.00, 1.00)
		offset = offset - 30
	end

	if showReplaysSaving then
		if (RAYGUI.GuiButton(10, offset, screenWidth-20, 20, "Save")) then
			showReplaysSaving = false

			local text = Api.StringFromData(Data["ReplayName_GuiTextBox"])

			if text ~= "" then
				Replay.Export(text .. ".bin")
			else
				Replay.Export("utitiledreplayfile.bin")
			end

			Api.StringToData("", Data["ReplayName_GuiTextBox"])
		end

		offset = offset - 30

		local data = Data["ReplayName_GuiTextBox"]
		local edit = editValue["ReplayName_GuiTextBox"]

		if RAYGUI.GuiTextBox(10, offset, screenWidth-20, 20, data, 1024, edit) then
			editValue["ReplayName_GuiTextBox"] = editValue["ReplayName_GuiTextBox"] == false
		end

		offset = offset - 30
	end
end
end
