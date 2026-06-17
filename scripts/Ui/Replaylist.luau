local UiReplaylist = {}

local show = false

local list = {}

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

function UiReplaylist.Toggle()
    show = show == false
	if show then
		list = RAYLIB.LoadDirectoryFiles("replays")
	end
end

function Api.Draw.UiReplaylist()
    if show then
	    if RAYGUI.GuiWindowBox(10, 40, (screenWidth/4) - 20, screenHeight - 100, "Replays") then
			show = false
		end

        for i = 1, #list do
	        local replay = list[i]:gsub("replays[/\\]", "")

            RAYGUI.GuiLabel(20, 10 + 30 * (i + 1), 400, 20, replay)

            if not replay:match(".+%.txt") then
		        if RAYGUI.GuiButton(350, 10 + 30 * (i + 1), 25, 25, `#{RAYGUI["ICON_PLAYER_PLAY"]}#`) then
		            if replay:match("^[%.%w%-_]+%.rpl") then
                        --Replay.Import(replay)

                        --local mod = Replay.GetMod()

                        if mod == mod_name then
							if mod:match("^CONVERTED_.+") then
								--Api.Reset()
								--local mod_name = mod:gsub("CONVERTED_", "")
								--ms.loadmodTBM2(mod_name)
								--Game.ImportMod()
								--Game.NewGame()
							end
						end

                        --Game.EnterMode(Game["MODE_REPLAY"], true)
					end
	
					show = false
                end
            end
        end
	end
end

return UiReplaylist
