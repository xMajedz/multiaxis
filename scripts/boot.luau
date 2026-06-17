--     load console first so i can get errors logged

local Console = require "Ui/Console"

Console.InsertMessage(`version: {Game.Version}`)

--     insert the intro message into the chat

for k, v in require("intro") do
	Console.InsertMessage(k .. ": " .. v)
end

Console.ShowMessages()

--     load the menu
Console.Menu = require "Ui/Menu"
