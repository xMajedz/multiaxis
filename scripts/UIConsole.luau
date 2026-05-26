local UIConsole = { TextColor = RAYLIB["RAYWHITE"] }

local showConsole = false
local showMessages = false

local maxMessages = 32

local messages = {}
local commands = {}

local GuiTextBox = API.CreateData(1024)
local EditValue = false

function commands.loadscript(script_path) 
	API.loadscript(script_path)
end

function commands.loadmod(mod_path) 
	API.loadmod(mod_path)
end

function commands.restart() 
	Game.Restart()
end

function commands.clear() 
	messages = {}
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

function UIConsole.InsertMessage(msg)
    table.insert(messages, msg)
end

function UIConsole.ShowMessages()
    showMessages = true
end

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function API.Draw.UIConsole()
    local offset = screenHeight - 25
	
	if showConsole and RAYGUI.GuiTextBox(10, offset, screenWidth - 20, 20, GuiTextBox, 1024, EditValue) then
			--EditValue = EditValue == false
	end
	
	if showMessages then
	    local i = 1
        local max = #messages
		local diff = #messages - maxMessages
		if 0 < diff then
		    max = maxMessages
		else
			diff = 0
		end
        while i <= #messages - diff do
			RAYLIB.DrawText(messages[i + diff], 10, offset - 20 * (max - i + 1), 20, UIConsole["TextColor"])
			i = i + 1
		end
	end
end

function API.Update.UIConsole(dt)
	if RAYLIB.IsKeyPressed(RAYLIB["KEY_ENTER"]) then
		local s = API.StringFromData(GuiTextBox)

		if showConsole and s ~= "" then
			if Net then
				Net.ClientEcho(s)
			else
				API.log(s)
			end

			API.StringToData("", GuiTextBox)
		end

		showMessages = (showConsole and showMessages) == false
		showConsole = showConsole == false
		EditValue = showConsole
	end

    if not showConsole and RAYLIB.IsKeyPressed(RAYLIB["KEY_SLASH"]) then
		showMessages = true 
		showConsole = true
	    EditValue = true
	end
end

function API.Console.UIConsole(msg)
	if msg:match("^/.*") then
		local cmd = msg:gsub("^/", "")
		cmd = commands[cmd:match("^%w+")]
		cmd = cmd and cmd(msg:gsub("^/%w+%s*", ""))
	else
		table.insert(messages, msg)
		showMessages = true
	end
end

return UIConsole
