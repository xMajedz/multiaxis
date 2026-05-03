if API == nil then
    --
    --    API events
    --    any function under any of the given namespaces will be called at the given event
    --
    API["NewGame"] = {}
    API["Freeze"] = {}
    API["Step"] = {}
    API["Update"] = {}
    API["Draw"} = {}
    API["NearCallback"] = {}
    API["FileDropped"] = {}
    API["Console"] = {}

    --
    --    API enum
    --    shapes used internally and their enum value
    --
    API["SHAPE_BOX"]       = 0
    API["SHAPE_SPHERE"]    = 1
    API["SHAPE_CAPSULE"]   = 2
    API["SHAPE_CYLINDER"]  = 3
    API["SHAPE_COMPOSITE"] = 4

    --
    --    API enum
    --    joint types used internall and their enum value
    --
    API["JOINT_NULL"]      = 0

    API["JOINT_BALL"]      = 1
    API["JOINT_HINGE"]     = 2
    API["JOINT_SLIDER"]    = 3,

	API["JOINT_UNIVERSAL"] = 4,
	API["JOINT_HINGE2"]    = 5,

    API["JOINT_FIXED"]     = 6,
    API["JOINT_CONTACT"]   = 7,

    --
	--    takes a size and returns a lightuserdata
	--
	function API.CreateData(size) end
	
	--
	--    sets the lightuserdata value to the given string
	--
	function API.StringToData(str, data) end

	--
	--    sets the lightuserdata value to the given number
	--
	function API.NumberToData(num, data) end

	--
	--    sets the lightuserdata value to the given byte
	--
	function API.ByteToData(byte, data) end

	--
	--    gets the lightuserdata value as a string 
    --
	function API.StringFromData(data) end

	--
	--    gets the lightuserdata value as a number 
    --
	function API.NumberFromData(data) end

	--
    --    gets the lightuserdata vallue as a byte
	--
	function API.ByteFromData(data) end

	--
	--    triggers the internal logging function
	--
    function API.log(str) end

	--
	--    resets the API state machine
	--
    function API.Reset() end

	--
	--    a standard implementation of the lua dofile function
	--
    function API.dofile(filename) end

	--
	--    a standard implementation fo the lua require function
	--
	function API.require(filename) end

	--
	--    loads a script from file
	--
	function API.loadscript(filename) end

	--
	--    loads a mod from file
	--
    function API.loadmod(filename) end
	
    --
	-- not implemented, should be a better way to parse mods as tables instead of the current state machine design
	--
	function API.loadmod_t(mod) end
end
