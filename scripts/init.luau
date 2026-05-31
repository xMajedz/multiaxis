local require = Api["require"]

--     load console first so i can get errors logged
local UiConsole = require "Ui/UiConsole"

--     insert the intro message into the chat
for k, v in require("intro") do
	UiConsole.InsertMessage(k .. ": " .. v)
end

UiConsole.ShowMessages()

--     load the menu
local UiMenu = require "Ui/UiMenu"
