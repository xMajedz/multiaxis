local require = API["require"]


--     load console first so i can get errors logged
local UIConsole = require "UIConsole"

--     insert the intro message into the chat
for k, v in require("intro") do
	UIConsole.InsertMessage(k .. ": " .. v)
end

--UIConsole["showConsoleMessages"] = true
UIConsole.ShowMessages()

--     load the rest of the UI
local UIMenu = require "UIMenu"
