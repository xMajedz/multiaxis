local UiReplaysave = {}

local show = false

local text = { buffer = buffer.create(256), edit = false }

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function Api.Update.UiReplaysave()
	if RAYLIB.IsKeyPressed(RAYLIB["KEY_F"]) then
		show = show == false
		Replay.Export("tempreplayfile.bin")
	end
end

function Api.Draw.UiReplaysave()
    local offset = screenHeight - 30

    if show then
		if (RAYGUI.GuiButton(10, screenHeight - 30, screenWidth-20, 20, "Save")) then
			show = false

			local content = buffer.readstring(text.buffer, 0, buffer.len(text.buffer))

			if content ~= "" then
				Replay.Export(content .. ".bin")
			else
				Replay.Export("utitiledreplayfile.bin")
			end

			buffer.writeu8(text.buffer, 0, 0)
		end

		offset = offset - 30

		if RAYGUI.GuiTextBox2(10, offset, screenWidth - 20, 20, text.buffer, text.edit) then
			text.edit = text.edit == false
		end

		offset = offset - 30
	end
end

return UiReplaysave