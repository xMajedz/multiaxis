local UiReplayslider = {}

local frame, max_frame, slidervalue = 0, 0, buffer.create(4)

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function Api.Update.UiReplayslider()
    frame = Game.GetGameFrame()
	max_frame = Replay.GetMaxFrame()
	if Game.IsPause() and Game.IsMode(Game["MODE_REPLAY"]) and Game.ReplayCacheEnabled() then
	    local value = buffer.readf32(slidervalue, 0)
	   
		Game.SetGameFrame(value * Replay.GetMaxFrame())
	end
end

function Api.Draw.UiReplayslider()
    if Game.IsMode(Game["MODE_REPLAY"])then
	    buffer.writef32(slidervalue, 0, frame/max_frame)
	   
		RAYGUI.GuiSliderBar(10, screenHeight - 20, screenWidth - 20, 10, "", "", slidervalue, 0.00, 1.00)
	end
end

return UiReplayslider
