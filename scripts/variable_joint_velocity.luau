local gear_count = 5
local current_gear_alt = gear_count
local current_gear = gear_count
local value = Api.CreateData(4)

Api.NumberToData(1.00, value)

function Api.Update.JVJ(dt)
	if Game.IsFreeze() and Game.IsMode(Game["MODE_FREEPLAY"]) and Game.IsSelectedJointValid() then
		if not RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_CONTROL"]) and RAYLIB.IsKeyPressed(RAYLIB["KEY_Z"]) then
			Game.Refreeze()

			local ratio = Api.NumberFromData(value)

			if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				Expermental.ToggleSelectedJointActiveStateAlt(ratio * Game.GetSelectedJointVelocityAlt())
			else
				Expermental.ToggleSelectedJointActiveState(ratio * Game.GetSelectedJointVelocity())
			end
		end

		if RAYLIB.IsKeyPressed(RAYLIB["KEY_N"]) then
			Game.Refreeze()

			if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				if current_gear_alt > 1 then
					current_gear_alt = current_gear_alt - 1
				end

				local ratio = current_gear_alt/gear_count

				Api.NumberToData(ratio, value)

				Expermental.TriggerSelectedJointActiveStateAlt(Game.GetSelectedJointVelocityAlt() * ratio)
			else
				if current_gear > 1 then
					current_gear = current_gear - 1
				end

				local ratio = current_gear/gear_count

				Api.NumberToData(ratio, value)

				Expermental.TriggerSelectedJointActiveState(Game.GetSelectedJointVelocity() * ratio)
			end

		end

		if RAYLIB.IsKeyPressed(RAYLIB["KEY_M"]) then
			Game.Refreeze()

			if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
				if current_gear_alt < gear_count  then
					current_gear_alt = current_gear_alt + 1
				end

				local ratio = current_gear_alt/gear_count

				Api.NumberToData(ratio, value)

				Expermental.TriggerSelectedJointActiveStateAlt(Game.GetSelectedJointVelocityAlt() * ratio)
			else
				if current_gear < gear_count then
					current_gear = current_gear + 1
				end

				local ratio = current_gear/gear_count

				Api.NumberToData(ratio, value)

				Expermental.TriggerSelectedJointActiveState(Game.GetSelectedJointVelocity() * ratio)
			end
		end
	end
    
    if Game.IsFreeze() and Game.IsMode(Game["MODE_FREEPLAY"]) and Game.IsSelectedPlayerValid() then
        local WheelMove = RAYLIB.GetMouseWheelMove()
        
        if WheelMove > 0 then
		    Game.Refreeze()

            if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
                if current_gear_alt < gear_count then
				    current_gear_alt = current_gear_alt + 1
			    end

			    local ratio = current_gear_alt/gear_count

			    Api.NumberToData(ratio, value)
				
                --Expermental.TriggerSelectedJointActiveStateAlt(Game.GetSelectedJointVelocityAlt() * ratio)
            else
                if current_gear < gear_count then
		            current_gear = current_gear + 1
			    end

			    local ratio = current_gear/gear_count

			    Api.NumberToData(ratio, value)

                --Expermental.TriggerSelectedJointActiveState(Game.GetSelectedJointVelocity() * ratio)
            end
		elseif WheelMove < 0 then
		    Game.Refreeze()
			
			if RAYLIB.IsKeyDown(RAYLIB["KEY_LEFT_SHIFT"]) then
			    if current_gear_alt > 1 then
				    current_gear_alt = current_gear_alt - 1
		        end

		        local ratio = current_gear_alt/gear_count

		        Api.NumberToData(ratio, value)
				
			    --Expermental.TriggerSelectedJointActiveStateAlt(Game.GetSelectedJointVelocityAlt() * ratio)
			else
			    if current_gear > 1 then
				    current_gear = current_gear - 1
		        end

			    local ratio = current_gear/gear_count

		        Api.NumberToData(ratio, value)
				
				--Expermental.TriggerSelectedJointActiveState(Game.GetSelectedJointVelocity() * ratio)
		    end
		end
	end
end


function Api.Draw.VJV()
	local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

	if not Game.IsMode(Game["MODE_REPLAY"]) then
		if RAYGUI.GuiSlider(screenWidth/4, screenHeight - 80, screenWidth/2, 20, "Min", "Max", value, 0.00, 1.00) then
			Game.Refreeze()
		end
	end
end
