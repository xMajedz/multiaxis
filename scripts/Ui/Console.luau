local Console = { Menu = { showUi = true }, textColor = RAYGUI.ColorToInt(RAYLIB["RAYWHITE"]) }

local csize = require "csize"

RAYGUI.GuiSetStyle(0, 16, 20)

local showConsole, showMessages = false, false

local maxMessages = 32

local messages, commands = {}, {}

local text, edit = buffer.create(csize.u8[256]), false

function commands.loadscript(script_path) 
	loadscript(script_path)
end

function commands.loadmod(mod_path) 
	loadmod(mod_path)
end

function commands.restart() 
	Game.Restart()
end

function commands.quit() 
	Game.Quit()
end

function commands.clear() 
	table.clear(messages)
end

function commands.log(arg)
    arg = arg:split(" ")
	arg = (arg[1] == "") or log(unpack(arg))
end

function commands.whisper(arg) 
if Net then
	local args = arg:split(" ")
	Net.ClientWhisper(tonumber(args[1]), args[2])
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
commands["qu"] = commands["quit"]

function Console.SetCommandAlias(alias, command)
    commands[alias] = commands[command]
end

function Console.SetCommand(command, func)
    commands[command] = func;
end

function Console.InsertMessage(msg)
    table.insert(messages, msg)
end

function Console.ShowMessages()
    showMessages = true
end

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

local function readcstring(text)
   local byte, i, s = buffer.readu8(text, 0), 0, ""
   while byte ~= 0 do
	  s = s .. string.char(byte)
	  i += 1
	  byte = buffer.readu8(text, i)
   end
   return s
end

Api.SetHotKey(RAYLIB["KEY_ENTER"], function()
	--local message = buffer.readstring(text, 0, buffer.len(text))
	local message = readcstring(text)
    if showConsole and message ~= "" then
	    local res = (Net and Net.ClientEcho(message)) or log(message)		
        buffer.writeu8(text, 0, 0)
    else
	    showMessages = (showConsole and showMessages) == false
	end
        
	showConsole = showConsole == false
	edit = showConsole
end)

Api.SetHotKey(RAYLIB["KEY_SLASH"], function()
	if not showConsole then
		showMessages = true
		showConsole = true
	    edit = true
	end
end)

function Api.Draw.Console()
if Console.Menu.showUi then
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
		RAYGUI.GuiSetStyle(0, 2, Console["textColor"])
        while i <= #messages - diff do
		    RAYGUI.GuiLabel(10, offset - 25 * (max - i + 1), screenWidth - 20, 25, messages[i + diff])
			i += 1
		end
		RAYGUI.GuiSetStyle(0, 2, textColor)
	end
end
end

function Api.Console.Console(msg)
	if msg:match("^/.*") then
		local cmd = msg:gsub("^/", "")
		cmd = commands[cmd:match("^%w+")]
		cmd = (cmd and cmd(msg:gsub("^/%w+%s*", ""))) or table.insert(messages, `Undefined command: {msg}`)   
	else
		table.insert(messages, msg)
		showMessages = true
	end
end

return Console
