local mesh    = Expermental.GenMeshPlane(340/45, 90/45, 340, 90)
local model   = Expermental.LoadModelFromMesh(mesh)
local texture = Expermental.LoadTexture("scripts/von/von.png")

Expermental.SetModelTexture(model, texture)
--[[
function Api.Draw.von()
    Expermental.DrawTexture(texture, 0, 0, RAYLIB["WHITE"])
end
]]
function Api.Draw3D.von()
    Expermental.DrawModel(model)
end
