local mesh    = Expermental.GenMeshPlane(340/45, 90/45, 340, 90)
local model   = Expermental.LoadModelFromMesh(mesh)
local texture = Expermental.LoadTexture("scripts/von/von.png")

Expermental.SetModelTexture(model, texture)

function API.Draw3D.von()
    Expermental.DrawModel(model)
end
