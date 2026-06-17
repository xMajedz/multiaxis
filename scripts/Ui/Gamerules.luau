local Gamerules = { Menu = {showUi = true} }

local screenWidth, screenHeight = RAYLIB.GetScreenWidth(), RAYLIB.GetScreenHeight()

local show = false
local mod, rules

local gravityX = { buffer = buffer.create(256), edit = false }
local gravityY = { buffer = buffer.create(256), edit = false }
local gravityZ = { buffer = buffer.create(256), edit = false }

local turnframes = { buffer = buffer.create(256), edit = false }
local reactiontime = { buffer = buffer.create(256), edit = false }

local numplayers = { buffer = buffer.create(256), edit = false }

local engagedistance = { buffer = buffer.create(256), edit = false }
local engageheight = { buffer = buffer.create(256), edit = false }

local maxcontacts = { buffer = buffer.create(256), edit = false }

local friction = { buffer = buffer.create(256), edit = false }
local bounce = { buffer = buffer.create(256), edit = false }

function Gamerules.Toggle()
    show = show == false
end

function Api.NewGame.Gamerules()
    mod = Game.GetMod()
    rules = Game.GetGamerules()

	if rules then
	    buffer.writestring(gravityX.buffer, 0, tostring(rules.gravity.x))
	    buffer.writestring(gravityY.buffer, 0, tostring(rules.gravity.y))
	    buffer.writestring(gravityZ.buffer, 0, tostring(rules.gravity.z))
	
	    buffer.writestring(turnframes.buffer, 0, tostring(rules.turnframes))
	    buffer.writestring(reactiontime.buffer, 0, tostring(rules.reaction_time))
	
	    buffer.writestring(numplayers.buffer, 0, tostring(rules.numplayers))

	    buffer.writestring(engagedistance.buffer, 0, tostring(rules.engagedistance))
	    buffer.writestring(engageheight.buffer, 0, tostring(rules.engageheight))

	    buffer.writestring(maxcontacts.buffer, 0, tostring(rules.max_contacts))

	    buffer.writestring(friction.buffer, 0, tostring(rules.friction))
	    buffer.writestring(bounce.buffer, 0, tostring(rules.bounce))
	end
end

function Api.Draw.Gamerules()
if Gamerules.Menu.showUi then
    if show then
	    if RAYGUI.GuiWindowBox(10, 40, (screenWidth/4) - 20, screenHeight - 100, `Gamerules: {mod}`) then
			show = false
		end

		if rules then
		    RAYGUI.GuiLabel(20, 75, 200, 40, "Gravity")

		    if RAYGUI.GuiTextBox(20, 110, 60, 20, gravityX.buffer, gravityX.edit) then
		        GravityX.edit = GravityX.edit == false
		    end

		    if RAYGUI.GuiTextBox(85, 110, 60, 20, gravityY.buffer, gravityY.edit) then
			    GravityY.edit = GravityY.edit == false
		    end

		    if RAYGUI.GuiTextBox(150, 110, 60, 20, gravityZ.buffer, gravityZ.edit) then
			    GravityZ.edit = GravityZ.edit == false
		    end
		
  		    RAYGUI.GuiLabel(20, 125, 170, 40, "Max Contacts")
		    if RAYGUI.GuiTextBox(20, 160, 60, 20, maxcontacts.buffer, maxcontacts.edit) then
			    maxcontacts.edit = maxcontacts.edit == false
		    end

		    RAYGUI.GuiLabel(20, 175, 200, 40, "Friction")

		    if RAYGUI.GuiTextBox(20, 210, 60, 20, friction.buffer, friction.edit) then
			    friction.edit = friction.edit == false
		    end

		    RAYGUI.GuiLabel(20, 225, 200, 40, "Bounce")

		    if RAYGUI.GuiTextBox(20, 260, 60, 20, bounce.buffer, bounce.edit) then
			    bounce.edit = bounce.edit == false
		    end

		    RAYGUI.GuiLabel(20, 275, 200, 40, "Players")

		    if RAYGUI.GuiTextBox(20, 310, 60, 20, numplayers.buffer, numplayers.edit) then
			    numuplayers.edit = numplayers.edit == false
		    end

		    RAYGUI.GuiLabel(20, 325, 200, 40, "Engage Height")

		    if RAYGUI.GuiTextBox(20, 360, 60, 20, engageheight.buffer, engageheight.edit) then
			    engageheight.edit = engageheight.edit == false
		    end
		
		    RAYGUI.GuiLabel(20, 375, 200, 40, "Engage Distance")

		    if RAYGUI.GuiTextBox(20, 410, 60, 20, engagedistance.buffer, engagedistance.edit) then
			    engagedistance.edit = engagedistance.edit == false
		    end
		
		    RAYGUI.GuiLabel(220, 75, 200, 40, "Turn Frames")

		    if RAYGUI.GuiTextBox(220, 110, 60, 20, turnframes.buffer, turnframes.edit) then
			    turnframes.edit = turnframes.edit == false
		    end

		    RAYGUI.GuiLabel(220, 125, 200, 40, "Reaction Time")

		    if RAYGUI.GuiTextBox(220, 160, 60, 20, reactiontime.buffer, reactiontime.edit) then
			    reactiontime.edit = reactiontime.edit == false
		    end

		    if RAYGUI.GuiButton(220, screenHeight - 80, 100, 20, "Apply") then
		        local x = tonumber(buffer.readstring(gravityX.buffer, 0, buffer.len(gravityX.buffer)))
			    local y = tonumber(buffer.readstring(gravityY.buffer, 0, buffer.len(gravityY.buffer)))
                local z = tonumber(buffer.readstring(gravityZ.buffer, 0, buffer.len(gravityZ.buffer)))
			    Game.SetGravity(x, y, z)

			    local mc = tonumber(buffer.readstring(maxcontacts.buffer, 0, buffer.len(maxcontacts.buffer)))
			    Game.SetMaxContacts(mc)

			    local mu = tonumber(buffer.readstring(friction.buffer, 0, buffer.len(friction.buffer)))
			    Game.SetFriction(mu)

			    local bnc = tonumber(buffer.readstring(bounce.buffer, 0, buffer.len(bounce.buffer)))
			    Game.SetBounce(bnc)

			    local tf = tonumber(buffer.readstring(turnframes.buffer, 0, buffer.len(turnframes.buffer)))
			    rules.turnframes = tf
			    Game.SetTurnFrames(tf)

			    local rt = tonumber(buffer.readstring(reactiontime.buffer, 0, buffer.len(reactiontime.buffer)))
			    rules.reaction_time = rt
			    Game.SetReactionTime(rt)

			    if true then  Game.NewGame() end
            end
        else
		    RAYGUI.GuiLabel(20, 55, 200, 40, "No Mod Loaded")
	    end
    end
end
end

return Gamerules
