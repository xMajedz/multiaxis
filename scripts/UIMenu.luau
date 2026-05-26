local require = API["require"]
local ms = require("modstuff")

local showGui = true

local darkMode = false

local showMenu = false
local showRules = false
local showMods = false

local showReplays = false
local showReplaysSaving = false

local showScripts = false
local showSettings = false

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

local Data = {}
Data["GravityX_GuiTextBox"] = API.CreateData(1024)
Data["GravityY_GuiTextBox"] = API.CreateData(1024)
Data["GravityZ_GuiTextBox"] = API.CreateData(1024)

Data["Players_GuiTextBox"] = API.CreateData(1024)
Data["EngageHeight_GuiTextBox"] = API.CreateData(1024)
Data["EngageDistance_GuiTextBox"] = API.CreateData(1024)

Data["TurnFrames_GuiTextBox"] = API.CreateData(1024)
Data["ReactionTime_GuiTextBox"] = API.CreateData(1024)
Data["MaxContacts_GuiTextBox"] =  API.CreateData(1024)
Data["Friction_GuiTextBox"] = API.CreateData(1024)
Data["Bounce_GuiTextBox"] = API.CreateData(1024)
Data["ReplayName_GuiTextBox"] = API.CreateData(1024)

Data["Settings_WindowWidth_GuiTextBox"] = API.CreateData(1024)
Data["Settings_WindowHeight_GuiTextBox"] = API.CreateData(1024)
Data["Settings_Fullscreen_GuiCheckBox"] = API.CreateData(1)

API.StringToData(tostring(screenWidth), Data["Settings_WindowWidth_GuiTextBox"])
API.StringToData(tostring(screenHeight) ,Data["Settings_WindowHeight_GuiTextBox"])

local editValue = {}
editValue["GravityX_GuiTextBox"] = false
editValue["GravityY_GuiTextBox"] = false
editValue["GravityZ_GuiTextBox"] = false

editValue["Players_GuiTextBox"] = false
editValue["EngageHeight_GuiTextBox"] = false
editValue["EngageDistance_GuiTextBox"] = false
editValue["TurnFrames_GuiTextBox"] = false
editValue["ReactionTime_GuiTextBox"] = false
editValue["MaxContacts_GuiTextBox"] =  false
editValue["Friction_GuiTextBox"] = false
editValue["Bounce_GuiTextBox"] = false

editValue["ReplayName_GuiTextBox"] = false
editValue["Settings_WindowWidth_GuiTextBox"] = false
editValue["Settings_WindowHeight_GuiTextBox"] = false

local replay_slider_value = API.CreateData(4)

local replay_list, mod_list, script_list
local mod, o_count, p_count, p_details, frame, contact_count, reaction_count, rules, gravity, console_message

local loaded_scripts = {}

RAYGUI.GuiSetStyle(0, 16, 20)

function API.NewGame.init()
	mod = Game.GetMod()
	o_count = Game.GetObjectCount()
	p_count = Game.GetPlayerCount()
	p_details = ""
	for i = 0, p_count - 1 do
		local b_count = Game.GetPlayerBodyCount(i)
		local j_count = Game.GetPlayerJointCount(i)
		p_details = p_details .. "p" .. (i + 1) .. ": "
		p_details = p_details .. "b:" .. b_count .. " "
		p_details = p_details .. "j:" .. j_count .. " "
	end

	rules = Game.GetGamerules()

	API.StringToData(`{rules.gravity.x}`, Data["GravityX_GuiTextBox"])
	API.StringToData(`{rules.gravity.y}`, Data["GravityY_GuiTextBox"])
	API.StringToData(`{rules.gravity.z}`, Data["GravityZ_GuiTextBox"])
	API.StringToData(`{rules.numplayers}`, Data["Players_GuiTextBox"])
	API.StringToData(`{rules.engageheight}`, Data["EngageHeight_GuiTextBox"])
	API.StringToData(`{rules.engagedistance}`, Data["EngageDistance_GuiTextBox"])
	API.StringToData(`{rules.turnframes}`, Data["TurnFrames_GuiTextBox"])
	API.StringToData(`{rules.reaction_time}`, Data["ReactionTime_GuiTextBox"])
	API.StringToData(`{rules.max_contacts}`, Data["MaxContacts_GuiTextBox"])
	API.StringToData(`{rules.friction}`, Data["Friction_GuiTextBox"])
	API.StringToData(`{rules.bounce}`, Data["Bounce_GuiTextBox"])
end

function API.Update.init(dt)
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
	
if Net then
else

	if RAYLIB.IsKeyPressed(RAYLIB["KEY_R"]) then
		Game.EnterMode(Game["MODE_REPLAY"])
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
		Game.EnterMode(Game["MODE_FREEPLAY"])
	end

	if Game.IsMode(Game["MODE_REPLAY"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_E"]) then
		Game.EnterMode(Game["MODE_REPLAY_EDIT"])
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

function API.Draw.init()
if showGui then
	if showMenu then
		local offset = 10
		local button_size = 100
		local gape = (screenWidth - 2 * button_size + 10)/3
		if RAYGUI.GuiButton(offset, 10, button_size, 25, "Rules") then
			showRules = showRules == false
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

	if showRules then
		if RAYGUI.GuiWindowBox(10, 40, 400, 400, `Rules: {mod}`) then
			showRules = false
		end

		if rules then
		--RAYGUI.GuiLabel(20, 60, screenWidth-10, 40, `{mod}: o:{o_count} p:{p_count} {p_details}`)

		RAYGUI.GuiLabel(20, 75, 200, 40, "Gravity")

		if RAYGUI.GuiTextBox(20, 110, 60, 20, Data["GravityX_GuiTextBox"], 1024, editValue["GravityX_GuiTextBox"]) then
			editValue["GravityX_GuiTextBox"] = editValue["GravityX_GuiTextBox"] == false
		end

		if RAYGUI.GuiTextBox(85, 110, 60, 20, Data["GravityY_GuiTextBox"], 1024, editValue["GravityY_GuiTextBox"]) then
			editValue["GravityY_GuiTextBox"] = editValue["GravityY_GuiTextBox"] == false
		end

		if RAYGUI.GuiTextBox(150, 110, 60, 20, Data["GravityZ_GuiTextBox"], 1024, editValue["GravityZ_GuiTextBox"]) then
			editValue["GravityZ_GuiTextBox"] = editValue["GravityZ_GuiTextBox"] == false
		end

		RAYGUI.GuiLabel(20, 125, 170, 40, "Max Contacts")

		if RAYGUI.GuiTextBox(20, 160, 60, 20, Data["MaxContacts_GuiTextBox"], 1024, editValue["MaxContacts_GuiTextBox"]) then
			editValue["MaxContacts_GuiTextBox"] = editValue["MaxContacts_GuiTextBox"] == false
		end
	
		RAYGUI.GuiLabel(20, 175, 200, 40, "Friction")

		if RAYGUI.GuiTextBox(20, 210, 60, 20, Data["Friction_GuiTextBox"], 1024, editValue["Friction_GuiTextBox"]) then
			editValue["Friction_GuiTextBox"] = editValue["Friction_GuiTextBox"] == false
		end

		RAYGUI.GuiLabel(20, 225, 200, 40, "Bounce")

		if RAYGUI.GuiTextBox(20, 260, 60, 20, Data["Bounce_GuiTextBox"], 1024, editValue["Bounce_GuiTextBox"]) then
			editValue["Bounce_GuiTextBox"] = editValue["Bounce_GuiTextBox"] == false
		end

		RAYGUI.GuiLabel(20, 275, 200, 40, "Players")

		if RAYGUI.GuiTextBox(20, 310, 60, 20, Data["Players_GuiTextBox"], 1024, editValue["Players_GuiTextBox"]) then
			editValue["Players_GuiTextBox"] = editValue["Players_GuiTextBox"] == false
		end

		RAYGUI.GuiLabel(20, 325, 200, 40, "Engage Height")

		if RAYGUI.GuiTextBox(20, 360, 60, 20, Data["EngageHeight_GuiTextBox"], 1024, editValue["EngageHeight_GuiTextBox"]) then
			editValue["EngageHeight_GuiTextBox"] = editValue["EngageHeight_GuiTextBox"] == false
		end
		
		RAYGUI.GuiLabel(20, 375, 200, 40, "Engage Distance")

		if RAYGUI.GuiTextBox(20, 410, 60, 20, Data["EngageDistance_GuiTextBox"], 1024, editValue["EngageDistance_GuiTextBox"]) then
			editValue["EngageDistance_GuiTextBox"] = editValue["EngageDistance_GuiTextBox"] == false
		end
		
		RAYGUI.GuiLabel(220, 75, 200, 40, "Turn Frames")

		if RAYGUI.GuiTextBox(220, 110, 60, 20, Data["TurnFrames_GuiTextBox"], 1024, editValue["TurnFrames_GuiTextBox"]) then
			editValue["TurnFrames_GuiTextBox"] = editValue["TurnFrames_GuiTextBox"] == false
		end

		RAYGUI.GuiLabel(220, 125, 200, 40, "Reaction Time")

		if RAYGUI.GuiTextBox(220, 160, 60, 20, Data["ReactionTime_GuiTextBox"], 1024, editValue["ReactionTime_GuiTextBox"]) then
			editValue["ReactionTime_GuiTextBox"] = editValue["ReactionTime_GuiTextBox"] == false
		end

		if RAYGUI.GuiButton(220, screenHeight-80, 100, 20, "Apply") then
			local x = tonumber(API.StringFromData(Data["GravityX_GuiTextBox"]))
			local y = tonumber(API.StringFromData(Data["GravityY_GuiTextBox"]))
			local z = tonumber(API.StringFromData(Data["GravityZ_GuiTextBox"]))
			Game.SetGravity(x, y, z)

			local mc = tonumber(API.StringFromData(Data["MaxContacts_GuiTextBox"]))
			Game.SetMaxContacts(mc)

			local friction = tonumber(API.StringFromData(Data["Friction_GuiTextBox"]))
			Game.SetFriction(friction)

			local bounce = tonumber(API.StringFromData(Data["Bounce_GuiTextBox"]))
			Game.SetBounce(bounce)

			local tf = tonumber(API.StringFromData(Data["TurnFrames_GuiTextBox"]))
			rules.turnframes = tf
			Game.SetTurnFrames(tf)

			local rt = tonumber(API.StringFromData(Data["ReactionTime_GuiTextBox"]))
			rules.reaction_time = rt
			Game.SetReactionTime(rt)

			if true then	
				Game.NewGame()
			end
		end

		else
			RAYGUI.GuiLabel(20, 55, 200, 40, "No Mod Loaded")
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
						API.Reset()
						API.loadmod(mod:match("^[%w%-_]+"))
						Game.Reset()
						Game.ImportMod()
						Game.NewGame()
					end
					if mod:match("^[%.%w%-_]+%.tbm") then
						API.Reset()
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
								API.Reset()
								local mod_name = mod:gsub("CONVERTED_", "")
								ms.loadmodTBM2(mod_name)
								Game.ImportMod()
								Game.NewGame()
							end
						end
						Game.EnterMode(Game["MODE_REPLAY"])
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

			if script:match(".+%.luau") then
				if (RAYGUI.GuiButton(350, 10 + 30 * (i + 1), 25, 25, `#{RAYGUI["ICON_PLAYER_PLAY"]}#`)) then
					script = script:gsub("%..+", "")

					API.loadscript(script)

					loaded_scripts[script] = 1

					showScripts = false
				end
			end
		end
	end

	if showSettings then
		if RAYGUI.GuiWindowBox(10, 40, 400, 400, "Settings") then
			showSettings = false
		end

		RAYGUI.GuiLabel(20, 70, 200, 20, "Resolution")

		if RAYGUI.GuiTextBox(120, 70, 90, 20, Data["Settings_WindowWidth_GuiTextBox"], 1024, editValue["Settings_WindowWidth_GuiTextBox"]) then
			editValue["Settings_WindowWidth_GuiTextBox"] = editValue["Settings_WindowWidth_GuiTextBox"] == false
		end

		if RAYGUI.GuiTextBox(215, 70, 90, 20, Data["Settings_WindowHeight_GuiTextBox"], 1024, editValue["Settings_WindowHeight_GuiTextBox"]) then
			editValue["Settings_WindowHeight_GuiTextBox"] = editValue["Settings_WindowHeight_GuiTextBox"] == false
		end

		if (RAYGUI.GuiButton(310, 70, 90, 20, "Apply")) then
			local width = API.StringFromData(Data["Settings_WindowWidth_GuiTextBox"])
			local height = API.StringFromData(Data["Settings_WindowHeight_GuiTextBox"])
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

        RAYGUI.GuiLabel(20, 170, 200, 20, "Ghost Cache")

        if (RAYGUI.GuiButton(310, 170, 90, 20, (Game.GhostCacheEnabled() and "Disable") or "Enable")) then
		    Game.ToggleGhostCache()
		end
	end

	local offset = screenHeight-30

	if Game.IsMode(Game["MODE_REPLAY"])then
		API.NumberToData(frame/Replay.GetMaxFrame(), replay_slider_value)
		RAYGUI.GuiSliderBar(10, offset, screenWidth-20, 20, "", "", replay_slider_value, 0.00, 1.00)
		offset = offset - 30
	end

	if showReplaysSaving then
		if (RAYGUI.GuiButton(10, offset, screenWidth-20, 20, "Save")) then
			showReplaysSaving = false

			local text = API.StringFromData(Data["ReplayName_GuiTextBox"])

			if text ~= "" then
				Replay.Export(text .. ".bin")
			else
				Replay.Export("utitiledreplayfile.bin")
			end

			API.StringToData("", Data["ReplayName_GuiTextBox"])
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
