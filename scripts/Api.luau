if Api == nil then
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

	--
	--    triggers the internal logging function
	--
    function Api.log(str) end

	--
	--    resets the Api state machine
	--
    function Api.Reset() end

	--
	--    a standard implementation of the lua dofile function
	--
    function Api.dofile(filename) end

	--
	--    a standard implementation fo the lua require function
	--
	function Api.require(filename) end

	--
	--    loads a script from file
	--
	function Api.loadscript(filename) end

	--
	--    loads a mod from file
	--
    function Api.loadmod(filename) end
	
    --
	-- not implemented, should be a better way to parse mods as tables instead of the current state machine design
	--
	function Api.loadmod_t(mod) end
end
