local Console = require "Ui/Console" -- load console first so i can get errors logged

runscript("redpixel.luau")

--[[
for i, message in require("intro") do -- insert the intro message into the chat
   Console.InsertMessage(`{message.lable}: {message.content}`)
end

Console.ShowMessages()

Console.Menu = require "Ui/Menu" -- load the menu
Console.Menu.Console = Console
]]
