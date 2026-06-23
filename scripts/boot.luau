--     load console first so i can get errors logged

local Console = require "Ui/Console"

--     insert the intro message into the chat

for i, message in require("intro") do
    Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

--     load the menu
Console.Menu = require "Ui/Menu"
Console.Menu.Console = Console