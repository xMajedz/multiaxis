local UiReplayslider = {}

local slidervalue = Api.CreateData(4)

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function Api.Update.UiReplayslider()
	if Game.IsPause() and Game.IsMode(Game["MODE_REPLAY"]) and Game.ReplayCacheEnabled() then
	    local value = Api.NumberFromData(slidervalue)
		Game.SetGameFrame(value * Replay.GetMaxFrame())
	end
end

function Api.Draw.UiReplayslider()
    if Game.IsMode(Game["MODE_REPLAY"])then
		Api.NumberToData(frame/Replay.GetMaxFrame(), slidervalue)
		RAYGUI.GuiSliderBar(10, screenHeight - 15, screenWidth - 20, 10, "", "", slidervalue, 0.00, 1.00)
		offset = offset - 30
	end
end

return UiReplayslider