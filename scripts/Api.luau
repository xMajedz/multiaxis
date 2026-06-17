if Api == nil then
    Api = {}
   
    --
    --    Api events
    --    any function under any of the given namespaces will be called at the given event
    --
    Api["NewGame"] = {}
    Api["Freeze"] = {}
    Api["Step"] = {}
    Api["Update"] = {}
    Api["Draw"} = {}
    Api["NearCallback"] = {}
    Api["FileDropped"] = {}
    Api["Console"] = {}

    --
    --    Api enum
    --    shapes used internally and their enum value
    --
    Api["SHAPE_BOX"]       = 0
    Api["SHAPE_SPHERE"]    = 1
    Api["SHAPE_CAPSULE"]   = 2
    Api["SHAPE_CYLINDER"]  = 3
    Api["SHAPE_COMPOSITE"] = 4

    --
    --    Api enum
    --    joint types used internall and their enum value
    --
    Api["JOINT_NULL"]      = 0

    Api["JOINT_BALL"]      = 1
    Api["JOINT_HINGE"]     = 2
    Api["JOINT_SLIDER"]    = 3,

	Api["JOINT_UNIVERSAL"] = 4,
	Api["JOINT_HINGE2"]    = 5,

    Api["JOINT_FIXED"]     = 6,
    Api["JOINT_CONTACT"]   = 7,
end
