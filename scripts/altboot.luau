-- should pass (table) to __call
Api.prototype()

-- should pass (table, key, value) to __newindex
function Api.prototype.newindex() end

-- should pass (table, key) to __index
local index = Api.prototype.index

Game.Quit()