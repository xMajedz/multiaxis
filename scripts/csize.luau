local csize, mt, size = {}, {}, 0

local list = {
    u8  = 1,
	
    i32 = 4,
	i64 = 8,

    f32 = 4,
	f64 = 8,
}

function mt.__index(self, key)
    if type(key) == "string" then
	    size = list[key] or 0
	    return self
	end
	if type(key) == "number" then
	    return key * size
	end
end

return setmetatable(csize, mt)