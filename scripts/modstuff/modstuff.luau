local require = API["require"]
local default = require("modstuff/default")
local fs = require("filestuff")
local Math = require("Math")

local ms = {}
ms["player_name"] = {}
ms["player_name"][1] = "ichiji"
ms["player_name"][2] = "niji"
ms["player_name"][3] = "sanji"
ms["player_name"][4] = "yonji"
ms["player_name"][5] = "goji"
ms["player_name"][6] = "rokuji"

ms["player_color"] = {}
ms["player_color"][1] = RAYLIB["RED"]
ms["player_color"][2] = RAYLIB["BLUE"]
ms["player_color"][3] = RAYLIB["GREEN"]
ms["player_color"][4] = RAYLIB["MAGENTA"]
ms["player_color"][5] = RAYLIB["YELLOW"]
ms["player_color"][6] = RAYLIB["PINK"]

ms["SHAPE_MAP"] = {}
ms["SHAPE_MAP"]["box"] = API["SHAPE_BOX"]
ms["SHAPE_MAP"]["sphere"] = API["SHAPE_SPHERE"]
ms["SHAPE_MAP"]["cylinder"] = API["SHAPE_CAPSULE"]

function ms.clone_player(p)
	local clone = {}
	clone = table.clone(p)
	clone["body"] = table.clone(p["body"])
	clone["joint"] = table.clone(p["joint"])

	for b_name, b in p["body"] do
		clone["body"][b_name] = table.clone(p["body"][b_name])
		clone["body"][b_name]["pos"] = table.clone(p["body"][b_name]["pos"])
	end
	for j_name, j in p["joint"] do
		clone["joint"][j_name] = table.clone(p["joint"][j_name])
		clone["joint"][j_name]["pos"] = table.clone(p["joint"][j_name]["pos"])
		if type(p["joint"][j_name]["axis"]) == "string" then
			p["joint"][j_name]["axis"] = p["joint"][j_name]["axis"]:split(" ")
		end
		clone["joint"][j_name]["axis"] = table.clone(p["joint"][j_name]["axis"])
	end
	return clone
end

function ms.player_set_origin(p)
	local x, y, z = ms.calculate_player_offset(p)
	ms.set_player_offset(p, -x, -y, -z)
end

function ms.rotateX_joint_axis(p, j_name, degree)
	local q = RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateX(RAYMATH["DEG2RAD"]*degree))
	if type(p["joint"][j_name]["axis"]) == "string" then
		p["joint"][j_name]["axis"] = p["joint"][j_name]["axis"]:split(" ")
	end
	local j_axis = p["joint"][j_name]["axis"]

	p["joint"][j_name]["axis"] = RAYMATH.Vector3RotateByQuaternion(j_axis, q)
end

function ms.rotateY_joint_axis(p, j_name, degree)
	local q = RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateY(RAYMATH["DEG2RAD"]*degree))
	if type(p["joint"][j_name]["axis"]) == "string" then
		p["joint"][j_name]["axis"] = p["joint"][j_name]["axis"]:split(" ")
	end
	local j_axis = p["joint"][j_name]["axis"]

	p["joint"][j_name]["axis"] = RAYMATH.Vector3RotateByQuaternion(j_axis, q)
end

function ms.rotateZ_joint_axis(p, j_name, degree)
	local q = RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateZ(RAYMATH["DEG2RAD"]*degree))
	if type(p["joint"][j_name]["axis"]) == "string" then
		p["joint"][j_name]["axis"] = p["joint"][j_name]["axis"]:split(" ")
	end
	local j_axis = p["joint"][j_name]["axis"]

	p["joint"][j_name]["axis"] = RAYMATH.Vector3RotateByQuaternion(j_axis, q)
end

function ms.rotateX_body_around_joint(p, b_name, j_name, degree)
	local q = RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateX(RAYMATH["DEG2RAD"]*degree))
	local j_pos = p["joint"][j_name]["pos"]
	local b_pos = p["body"][b_name]["pos"]
	
	b_pos[1] = b_pos[1] - j_pos[1]
	b_pos[2] = b_pos[2] - j_pos[2]
	b_pos[3] = b_pos[3] - j_pos[3]
	
	b_pos = RAYMATH.Vector3RotateByQuaternion(b_pos, q)
	
	b_pos[1] = b_pos[1] + j_pos[1]
	b_pos[2] = b_pos[2] + j_pos[2]
	b_pos[3] = b_pos[3] + j_pos[3]
	
	p["body"][b_name]["pos"] = b_pos
end

function ms.rotateY_body_around_joint(p, b_name, j_name, degree)
	local q = RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateY(RAYMATH["DEG2RAD"]*degree))
	local j_pos = p["joint"][j_name]["pos"]
	local b_pos = p["body"][b_name]["pos"]
	
	b_pos[1] = b_pos[1] - j_pos[1]
	b_pos[2] = b_pos[2] - j_pos[2]
	b_pos[3] = b_pos[3] - j_pos[3]
	
	b_pos = RAYMATH.Vector3RotateByQuaternion(b_pos, q)
	
	b_pos[1] = b_pos[1] + j_pos[1]
	b_pos[2] = b_pos[2] + j_pos[2]
	b_pos[3] = b_pos[3] + j_pos[3]
	
	p["body"][b_name]["pos"] = b_pos
end

function ms.rotateZ_body_around_joint(p, b_name, j_name, degree)
	local q = RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateZ(RAYMATH["DEG2RAD"]*degree))
	local j_pos = p["joint"][j_name]["pos"]
	local b_pos = p["body"][b_name]["pos"]
	
	b_pos[1] = b_pos[1] - j_pos[1]
	b_pos[2] = b_pos[2] - j_pos[2]
	b_pos[3] = b_pos[3] - j_pos[3]
	
	b_pos = RAYMATH.Vector3RotateByQuaternion(b_pos, q)
	
	b_pos[1] = b_pos[1] + j_pos[1]
	b_pos[2] = b_pos[2] + j_pos[2]
	b_pos[3] = b_pos[3] + j_pos[3]
	
	p["body"][b_name]["pos"] = b_pos
end

function ms.set_player_engagedistance(TBM, p, p_idx)
	local angle = (360/TBM["numplayers"]) * (p_idx - 1)
	local dir = {0.00, 1.00, 0.00}
	local q = RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateZ(RAYMATH["DEG2RAD"]*angle))
	local v = {dir[1]*TBM["engagedistance"], dir[2]*TBM["engagedistance"], dir[3]*TBM["engagedistance"]}
	local pos = RAYMATH.Vector3RotateByQuaternion(v, q)
	for _, b in p["body"] do
		local E2Q, DEG2RAD = ms["E2Q"], RAYMATH["DEG2RAD"]
		local rot = b["rot"] or {0.00, 0.00, 0.00}
		b["qat"] = E2Q(rot[1] * DEG2RAD, rot[2] * DEG2RAD, rot[3] * DEG2RAD)
		b["qat"] = RAYMATH.QuaternionMultiply(q, b["qat"])
		b["pos"] = RAYMATH.Vector3RotateByQuaternion(b["pos"], q)
		b["pos"][1] = b["pos"][1] + pos[1]
		b["pos"][2] = b["pos"][2] + pos[2]
		b["pos"][3] = b["pos"][3] + pos[3]
	end
	for _, j in p["joint"] do
		j["pos"] = RAYMATH.Vector3RotateByQuaternion(j["pos"], q)
		j["pos"][1] = j["pos"][1] + pos[1]
		j["pos"][2] = j["pos"][2] + pos[2]
		j["pos"][3] = j["pos"][3] + pos[3]
	end
end

function ms.set_player_engageheight(TBM, p)
	for _, b in p["body"] do
		b["pos"][3] = b["pos"][3] + TBM["engageheight"]
	end
	for _, j in p["joint"] do
		j["pos"][3] = j["pos"][3] + TBM["engageheight"]
	end
end

function ms.set_player_offset(p, x, y, z)
	local v = {x, y, z}
	for _, b in p["body"] do
		b["pos"][1] = b["pos"][1] + v[1]
		b["pos"][2] = b["pos"][2] + v[2]
		b["pos"][3] = b["pos"][3] + v[3]
	end
	for _, j in p["joint"] do
		j["pos"][1] = j["pos"][1] + v[1]
		j["pos"][2] = j["pos"][2] + v[2]
		j["pos"][3] = j["pos"][3] + v[3]
	end
end

function ms.calculate_player_offset(p)
	local sum = {0.00, 0.00, 0.00}
	local count = 0
	for _, b in p["body"] do
		sum[1] = sum[1] + b["pos"][1]
		sum[2] = sum[2] + b["pos"][2]
		sum[3] = sum[3] + b["pos"][3]
		count = count + 1
	end
	for _, j in p["joint"] do
		sum[1] = sum[1] + j["pos"][1]
		sum[2] = sum[2] + j["pos"][2]
		sum[3] = sum[3] + j["pos"][3]
		count = count + 1
	end
	return sum[1]/count, sum[2]/count, sum[3]/count
end

function ms.normalize_player_offset(v)
	local magnitude = math.sqrt(v[1]*v[1] + v[2]*v[2] + v[3]*v[3])
	return {v[1]/magnitude, v[2]/magnitude, v[3]/magnitude}
end

function ms.scale_player_joint_strength(p, factor)
	for _, j in p["joint"] do
		j["strength"] = j["strength"] * factor
	end
end

function ms.scale_player_joint_strength_alt(p, factor)
	for _, j in p["joint"] do
		j["strength_alt"] = j["strength_alt"] * factor
	end
end

function ms.E2Q(u, v, w)
	return Math.MatrixToQuaternion(Math.EulerToMatrix(u, v, w))
end

function ms.E2Q2(u, v, w)
	return RAYMATH.QuaternionFromMatrix(RAYMATH.MatrixRotateXYZ({u, v, w}))
end

function ms.loadmodTBM(modname)
	ms.loadTBM(ms.parseTBM(modname))
end

function ms.loadmodTBM2(modname)
	local TBM = ms.parseTBM(modname)

	TBM["mod"] = "CONVERTED_" .. modname
	TBM["numplayers"] = 2
	TBM["engageheight"] = 3.40
	TBM["engagedistance"] = 2.36
	
	TBM["player"][1]["engagepos"] = nil
	TBM["player"][1]["body"]["l_hand"]["flag"] = { interactive = true }
	TBM["player"][1]["body"]["r_hand"]["flag"] = { interactive = true }
	
	ms.rotateY_body_around_joint(TBM["player"][1], "l_biceps", "l_shoulder", -90)
	ms.rotateY_body_around_joint(TBM["player"][1], "r_biceps", "r_shoulder", 90)
	
	ms.rotateY_joint_axis(TBM["player"][1], "neck", 180)
	ms.rotateY_joint_axis(TBM["player"][1], "abs", 180)
	
	ms.rotateY_joint_axis(TBM["player"][1], "l_hip", 180)
	ms.rotateY_joint_axis(TBM["player"][1], "l_knee", 180)
	ms.rotateY_joint_axis(TBM["player"][1], "l_ankle", 180)
	
	ms.rotateY_joint_axis(TBM["player"][1], "r_hip", 180)
	ms.rotateY_joint_axis(TBM["player"][1], "r_knee", 180)
	ms.rotateY_joint_axis(TBM["player"][1], "r_ankle", 180)
	
	ms.rotateY_joint_axis(TBM["player"][1], "r_pecs", 180)
	
	ms.rotateY_joint_axis(TBM["player"][1], "r_elbow", 180)
	
	ms.rotateY_joint_axis(TBM["player"][1], "r_wrist", 180)
	
	ms.rotateZ_joint_axis(TBM["player"][1], "r_glute", 180)
	
	for i = 2, TBM["numplayers"] do
		TBM["player"][i] = ms.clone_player(TBM["player"][1])
	end
	
	for p_idx, p in TBM["player"] do
		ms.player_set_origin(p)
		ms.set_player_engagedistance(TBM, p, p_idx)
		ms.set_player_engageheight(TBM, p)
	end
	
	local offset = {0.00, 0.00, 0.00}
	local count = 0
	for o_idx, o in TBM["env_obj"] do
		if type(o["pos"]) == "string" then
			o["pos"] = o["pos"]:split(" ")
		end
		offset[1] = offset[1] + o["pos"][1]
		offset[2] = offset[2] + o["pos"][2]
		count = count + 1
	end
	for oj_idx, oj in TBM["env_obj_joint"] do
		if type(oj["pos"]) == "string" then
			oj["pos"] = oj["pos"]:split(" ")
		end
		offset[1] = offset[1] + oj["pos"][1]
		offset[2] = offset[2] + oj["pos"][2]
		count = count + 1
	end
	offset[1] = offset[1]/count
	offset[2] = offset[2]/count
	for o_idx, o in TBM["env_obj"] do
		o["pos"][1] = o["pos"][1] - offset[1]
		o["pos"][2] = o["pos"][2] - offset[2]
	end
	for oj_idx, oj in TBM["env_obj_joint"] do
		oj["pos"][1] = oj["pos"][1] - offset[1]
		oj["pos"][2] = oj["pos"][2] - offset[2]
	end
	
	ms.loadTBM(TBM)
end

function ms.loadModifiedTBM(fn)
	ms.loadTBM(fn())
end

function ms.loadTable()
	API["loadmod_t"] {
		["mod"] = "string",
		["turnframes"] = 10,
		["reactiontime"] = 20,
		["numplayers"] = 20,
		["friction"] = 10000,
		["bounce"] = 10,
		["gravity"] = { 0.00, 0.00, 0.00 },
	}
end

function ms.loadTBM(TBM)
	local E2Q = ms["E2Q"]
	local DEG2RAD = RAYMATH["DEG2RAD"]

	API["mod"] (TBM["mod"])
	API["turnframes"] {TBM["turnframes"] + 0}
	API["reactiontime"] {TBM["reactiontime"] or 0}
	if TBM["engageheight"] then
		API["engageheight"] {TBM["engageheight"] + 0}
	end
	if TBM["engagedistance"] then
		API["engagedistance"] {TBM["engagedistance"] + 0}
	end
	API["numplayers"] {TBM["numplayers"] + 0}
	API["friction"] {TBM["friction"] or 1000.00}
	API["gravity"] {TBM["gravity"][1] + 0, TBM["gravity"][2] + 0, TBM["gravity"][3] + 0}
	for env_obj_num, env_obj in TBM["env_obj"] do
		API["object"](env_obj_num)
		API["shape"](env_obj["shape"] or API["SHAPE_BOX"])
		API["position"] {env_obj["pos"][1] + 0, env_obj["pos"][2] + 0, env_obj["pos"][3] + 0}
		if env_obj["rot"] then
			local q = E2Q(env_obj["rot"][1] * DEG2RAD, env_obj["rot"][2] * DEG2RAD, env_obj["rot"][3] * DEG2RAD)
			API["orientation"] {q[1], q[2], q[3], q[4]}
		end
		if env_obj["shape"] == API["SHAPE_SPHERE"] then
			API["radius"] {env_obj["radius"] + 0}
		elseif env_obj["shape"] == API["SHAPE_CAPSULE"] then
			API["radius"] {env_obj["radius"] + 0}
			API["length"] {env_obj["length"] + 0}
		else
			API["sides"] {env_obj["sides"][1] + 0, env_obj["sides"][2] + 0, env_obj["sides"][3] + 0}
		end
		if env_obj["density"] then
			API["density"] {env_obj["density"] + 0}
		else
			API["density"] {0.25}
		end
		if env_obj["color"] then
			API["color"] {
				255 * (env_obj["color"][1] + 0),
				255 * (env_obj["color"][2] + 0),
				255 * (env_obj["color"][3] + 0),
				255 * (env_obj["color"][4] + 0),
			}
		end
		local flag = {}
		if env_obj["flag"] == "24" then
			flag["static"] = true
		end
		API["flag"](flag)
	end

	for env_obj_joint_num, env_obj_joint in TBM["env_obj_joint"] do
		API["objectjoint"](env_obj_joint_num)
		API["shape"](API["SHAPE_SPHERE"])
		API["radius"] {env_obj_joint["radius"]}
		if type(env_obj_joint["pos"]) == "string" then
			 env_obj_joint["pos"] = env_obj_joint["pos"]:split(" ")
		end
		API["position"] {env_obj_joint["pos"][1], env_obj_joint["pos"][2], env_obj_joint["pos"][3]}
		API["density"] {0.0025}
		API["strength"] {env_obj_joint["strength"]}
		if env_obj_joint["strength_alt"] then
			API["strength_alt"] {env_obj_joint["strength_alt"]}
		end
		API["velocity"] {env_obj_joint["velocity"]}
		if env_obj_joint["strength_alt"] then
			API["velocity_alt"] {env_obj_joint["velocity_alt"]}
		end
		if type(env_obj_joint["axis"]) == "string" then
			 env_obj_joint["axis"] = env_obj_joint["axis"]:split(" ")
		end
		if env_obj_joint["axis"] then
			API["axis"] {env_obj_joint["axis"][1], env_obj_joint["axis"][2], env_obj_joint["axis"][3]}
		else
			API["axis"] {1.00, 0.00, 0.00}
		end
		if env_obj_joint["axis_alt"] then
			API["axis_alt"] {env_obj_joint["axis_alt"][1], env_obj_joint["axis_alt"][2], env_obj_joint["axis_alt"][3]}
		end
		if type(env_obj_joint["range"]) == "string" then
			 env_obj_joint["range"] = env_obj_joint["range"]:split(" ")
		end
		if env_obj_joint["range"] then
			API["range"] {env_obj_joint["range"][1], env_obj_joint["range"][2]}
		else
			API["range"] {0.00, 0.00}
		end
		if env_obj_joint["range_alt"] then
			API["range_alt"] {env_obj_joint["range_alt"][1], env_obj_joint["range_alt"][2]}
		end
		if type(env_obj_joint["connections"]) == "string" then
			env_obj_joint["connections"] = env_obj_joint["connections"]:split(" ")
		end
		API["connections"] {env_obj_joint["connections"][1], env_obj_joint["connections"][2]}
		API["connection_type"](env_obj_joint["type"] or API["JOINT_HINGE"])
	end

	for player_name, p in TBM["player"] do
		API["player"](ms["player_name"][player_name])
		API["color"](ms["player_color"][player_name])
		--API["catbits"](ms["player_catbits"][player_name])
		--API["colbits"](ms["player_colbits"][player_name])
		if p["engagepos"] then
			API["engagepos"] {p["engagepos"][1] + 0, p["engagepos"][2] + 0, p["engagepos"][3] + 0}
		end
		if p["engagerot"] then
			API["engagerot"] {p["engagerot"][1] + 0, p["engagerot"][2] + 0, p["engagerot"][3] + 0}
		end
		for body_name, b in p["body"] do
			API["body"](body_name)
			API["shape"](b["shape"])
			if type(b["pos"]) == "string" then
				b["pos"] = b["pos"]:split(" ")
			end
			API["position"] {b["pos"][1] + 0, b["pos"][2] + 0, b["pos"][3] + 0}
			if b["qat"] then
				API["orientation"] {b["qat"][1], b["qat"][2], b["qat"][3], b["qat"][4]}
			elseif b["rot"] then
				local q = E2Q(b["rot"][1] * DEG2RAD, b["rot"][2] * DEG2RAD, b["rot"][3] * DEG2RAD)
				API["orientation"] {q[1], q[2], q[3], q[4]}
			end
			if body_name == "head" or b["shape"] == API["SHAPE_SPHERE"] then
				API["radius"] {b["radius"] + 0}
			elseif b["shape"] == API["SHAPE_CAPSULE"] then
				API["radius"] {b["radius"] + 0}
				API["length"] {b["length"] + 0}
			else
				if type(b["sides"]) == "string" then
					b["sides"] = b["sides"]:split(" ")
				end
				API["sides"] {b["sides"][1] + 0, b["sides"][2] + 0, b["sides"][3] + 0}
			end
			API["density"] {b["density"] + 0}
			API["flag"] (b["flag"] or {})
		end
		for joint_name, j in p["joint"] do
			API["joint"](joint_name)
			API["shape"](API["SHAPE_SPHERE"])
			API["radius"] {j["radius"]}
			if type(j["pos"]) == "string" then
				 j["pos"] = j["pos"]:split(" ")
			end
			API["position"] {j["pos"][1], j["pos"][2], j["pos"][3]}
			API["density"] {0.0025}
			API["strength"] {j["strength"]}
			if j["strength_alt"] then
				API["strength_alt"] {j["strength_alt"]}
			end
			API["velocity"] {j["velocity"]}
			if j["strength_alt"] then
				API["velocity_alt"] {j["velocity_alt"]}
			end
			if type(j["axis"]) == "string" then
				 j["axis"] = j["axis"]:split(" ")
			end
			API["axis"] {j["axis"][1], j["axis"][2], j["axis"][3]}
			if j["axis_alt"] then
				API["axis_alt"] {j["axis_alt"][1], j["axis_alt"][2], j["axis_alt"][3]}
			end
			if type(j["range"]) == "string" then
				 j["range"] = j["range"]:split(" ")
			end
			API["range"] {j["range"][1], j["range"][2]}
			if j["range_alt"] then
				API["range_alt"] {j["range_alt"][1], j["range_alt"][2]}
			end
			if type(j["connections"]) == "string" then
				j["connections"] = j["connections"]:split(" ")
			end
			API["connections"] {j["connections"][1], j["connections"][2]}
			API["connection_type"](j["type"] or API["JOINT_HINGE"])
			API["flag"] { composite = true }
		end
	end
end

function ms.parseTBM(filename)
	local file_lines = fs.readfile("mods/" .. filename:gsub(".tbm", "") .. ".tbm")
	local TBM = table.clone(default)
	TBM["player"][1] = table.clone(TBM["player"][1])
	TBM["player"][2] = table.clone(TBM["player"][2])
	local env_obj_num = ""
	local env_obj_joint_num = ""
	local player_name = ""
	local body_name = ""
	local joint_name = ""
	local context = ""
	local data = ""
	TBM["mod"] = filename
	for _, line in file_lines do
		if line:match("version ") then
			TBM["version"] = line:gsub(".*version ", "")
		elseif line:match("turnframes ") then
			TBM["turnframes"] = line:gsub(".*turnframes ", "");
		elseif line:match("numplayers ") then
			TBM["numplayers"] = line:gsub(".*numplayers ", "");
		elseif line:match("gravity ") then
			line = line:gsub(".*gravity ", "")
			TBM["gravity"] = line:split(" ")
		elseif line:match("engageplayerpos ") then
			line = line:gsub(".*engageplayerpos ", "")
			TBM["engageplayerpos"] = line:split(",")
			local k = 0
			for i = 1, #TBM["engageplayerpos"]/3 do
				TBM["player"][i]["engagepos"] = {}
				for j = 0, 2 do
					local index = i + j + k
					TBM["player"][i]["engagepos"][j + 1] = TBM["engageplayerpos"][index]
				end
				k = k + 2
			end
		elseif line:match("engageplayerrot ") then
			line = line:gsub(".*engageplayerrot ", "")
			TBM["engageplayerrot"] = line:split(",")
			local k = 0
			for i = 1, #TBM["engageplayerrot"]/3 do
				TBM["player"][i]["engagerot"] = {}
				for j = 0, 2 do
					local index = i + j + k
					TBM["player"][i]["engagerot"][j + 1] = TBM["engageplayerrot"][index]
				end
				k = k + 2
			end
		elseif line:match("player ") then
			player_name = line:gsub(".*player ", "") + 1
		elseif line:match("env_obj ") then
			context = "env_obj"
			data = line:gsub(".*env_obj ", "")
			TBM[context][data] = {}
		elseif line:match("env_obj_joint") then
			context = "env_obj_joint"
			data = line:gsub(".*env_obj_joint ", "")
			line = data:split(" ")
			data = line[1]
			TBM[context][line[1]] = {["connections"] = { line[2], line[3] }}
		elseif line:match("body ") then
			context = "body"
			data = line:gsub(".*body ", "")
		elseif line:match("joint ")  then
			context = "joint"
			data = line:gsub(".*joint ", "")
		elseif line:match("shape ") then
			line = line:gsub(".*shape ", "")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["shape"] = ms["SHAPE_MAP"][line]
			else
				TBM[context][data]["shape"] = ms["SHAPE_MAP"][line]
			end
		elseif line:match("pos ") then
			line = line:gsub(".*pos ", "")
			line = line:split(" ")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["pos"] = line
			else
				TBM[context][data]["pos"] = line
			end
		elseif line:match("rot ") then
			line = line:gsub(".*rot ", "")
			line = line:split(" ")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["rot"] = line
			else
				TBM[context][data]["rot"] = line
			end
		elseif line:match("sides ") then
			line = line:gsub(".*sides ", "")
			line = line:split(" ")
      if context == "body" or context == "joint" then
					if TBM["player"][player_name][context][data]["shape"] == API["SHAPE_SPHERE"] then
						TBM["player"][player_name][context][data]["radius"] = line[1]
					elseif TBM["player"][player_name][context][data]["shape"] == API["SHAPE_CAPSULE"] then
						TBM["player"][player_name][context][data]["radius"] = line[1]
						TBM["player"][player_name][context][data]["length"] = line[2]
					else
						TBM["player"][player_name][context][data]["sides"] = line
					end
			else
					if TBM[context][data]["shape"] == API["SHAPE_SPHERE"] then
						TBM[context][data]["radius"] = line[1]
					elseif TBM[context][data]["shape"] == API["SHAPE_CAPSULE"] then
						TBM[context][data]["radius"] = line[1]
						TBM[context][data]["length"] = line[2]
					else
						TBM[context][data]["sides"] = line
					end
      end
		elseif line:match("radius ") then
			line = line:gsub(".*radius ", "")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["radius"] = line
			else
				TBM[context][data]["radius"] = line
			end
		elseif line:match("density ") then
			line = line:gsub(".*density ", "")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["density"] = line
			else
				TBM[context][data]["density"] = line
			end
		elseif line:match("axis ") then
			line = line:gsub(".*axis ", "")
      line = line:split(" ")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["axis"] = line
			else
				TBM[context][data]["axis"] = line
			end
		elseif line:match("range ") then
			line = line:gsub(".*range ", "")
      line = line:split(" ")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["range"] = line
			else
				TBM[context][data]["range"] = line
			end
		elseif line:match("strength ") then
			line = line:gsub(".*strength ", "")
			if context == "body" or context == "joint" then
				TBM["player"][player_name][context][data]["strength"] = line
			else
				TBM[context][data]["strength"] = line
			end
		elseif line:match("velocity ") then
			line = line:gsub(".*velocity ", "")
			if context == "body" or context == "joint" then
			TBM["player"][player_name][context][data]["velocity"] = line 
			else
			TBM[context][data]["velocity"] = line 
			end
		elseif line:match("color ") then
			line = line:gsub(".*color ", "")
			if context == "body" or context == "joint" then
			TBM["player"][player_name][context][data]["color"] = line:split(" ") 
			else
			TBM[context][data]["color"] = line:split(" ") 
			end
		elseif line:match("flag ") then
			line = line:gsub(".*flag ", "")
			if context == "" then
				TBM["flag"] = line
			elseif context == "body" or context == "joint" then
					TBM["player"][player_name][context][data]["flag"] = line
			else
					TBM[context][data]["flag"] = line
			end
		end
	end
	return TBM
end

return ms
