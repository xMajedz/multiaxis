local uielement = require "@uielement"
local von = uielement.new {bgImage = "scripts/von/von.png", interactive = true}
function von.mouseButtonPressed()
    von:kill()
    von = nil
end
