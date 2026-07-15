local random, uielement, Ray = math.random, require "@UIElement", require "@Raylib"
local redpixel = uielement.new { pos = {random() * Ray.GetScreenWidth(), random() * Ray.GetScreenHeight()}, bgColor = uielement["UICOLORRED"] }
function Api.prototype.redpixel() local keepalive = redpixel end -- a little hack to keep the userdata alive with a global refrence.
