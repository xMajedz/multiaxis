local UiConsole = { textColor = RAYLIB["RAYWHITE"] }

local showConsole = false
local showMessages = false

local maxMessages = 32

local messages, commands = {}, {}

local text, edit = buffer.create(256), false

function commands.loadscript(script_path) 
	Api.loadscript(script_path)
end

function commands.loadmod(mod_path) 
	Api.loadmod(mod_path)
end

function commands.restart() 
	Game.Restart()
end

function commands.clear() 
	table.clear(messages)
end

function commands.whisper(arg) 
if Net then
	local arg = arg:split(" ")
	Net.ClientWhisper(tonumber(arg[1]), arg[2])
end
end

function commands.mods(mod_path) 
	local mods = RAYLIB.LoadDirectoryFiles("mods");
	table.insert(messages, #mods .. " mod")

	for i = 1, #mods do
		local mod = mods[i]:gsub("mods[/\\]", "")
		table.insert(messages, mod)
		showMessages = true
	end
end

commands["ls"] = commands["loadscript"]
commands["lm"] = commands["loadmod"]
commands["rt"] = commands["restart"]
commands["cl"] = commands["clear"]

function UiConsole.RegisterCommandAlias(alias, command)
    commands[alias] = commands[command]
end

function UiConsole.RegisterCommand(command, func)
    commands[command] = func;
end

function UiConsole.InsertMessage(msg)
    table.insert(messages, msg)
end

function UiConsole.ShowMessages()
    showMessages = true
end

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function Api.Draw.UiConsole()
    local offset = screenHeight - 25
	
	if showConsole and RAYGUI.GuiTextBox(10, offset, screenWidth - 20, 20, text, edit) then end
	
	if showMessages then
	    local i = 1
        local max = #messages
		local diff = #messages - maxMessages
		if 0 < diff then
		    max = maxMessages
		else
			diff = 0
		end
		local textColor = RAYGUI.GuiGetStyle(0, 2)
		RAYGUI.GuiSetStyle(0, 2, RAYGUI.ColorToInt(UiConsole["textColor"]))
        while i <= #messages - diff do
		    RAYGUI.GuiLabel(10, offset - 25 * (max - i + 1), screenWidth - 20, 25, messages[i + diff])
			i += 1
		end
		RAYGUI.GuiSetStyle(0, 2, textColor)
	end
end

function Api.Update.UiConsole(dt)
	if RAYLIB.IsKeyPressed(RAYLIB["KEY_ENTER"]) then
        local s = buffer.readstring(text, 0, buffer.len(text))
		
		if showConsole and buffer.readu8(text, 0) ~= 0 then
			if Net then
				Net.ClientEcho(s)
			else
				Api.log(s)
			end
			
            buffer.writeu8(text, 0, 0)
		end

		showMessages = (showConsole and showMessages) == false
		showConsole = showConsole == false
		edit = showConsole
	end

    if not showConsole and RAYLIB.IsKeyPressed(RAYLIB["KEY_SLASH"]) then
		showMessages = true 
		showConsole = true
	    edit = true
	end
end

function Api.Console.UiConsole(msg)
	if msg:match("^/.*") then
		local cmd = msg:gsub("^/", "")
		cmd = commands[cmd:match("^%w+")]
		cmd = cmd and cmd(msg:gsub("^/%w+%s*", ""))
	else
		table.insert(messages, msg)
		showMessages = true
	end
end

return UiConsole
