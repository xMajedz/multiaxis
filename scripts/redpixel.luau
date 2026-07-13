local random, uielement, Ray = math.random, require "@UIElement", require "@Raylib"
uielement.new { pos = {random() * Ray.GetScreenWidth(), random() * Ray.GetScreenHeight()}, bgColor = uielement["UICOLORRED"] }
