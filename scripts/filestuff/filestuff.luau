local LoadFileText, SaveFileText = RAYLIB["LoadFileText"], RAYLIB["SaveFileText"]

local fs = {}

local content, lines = "", {}

function fs.clear_content()
    content = ""
    lines = {}
end

function fs.readfile(pathtofile)
    if (pathtofile) then
	    content = LoadFileText(pathtofile)
	    content = content:gsub("\r", "")
		lines = content:split("\n")
        return lines
    end
end


function fs.writefile(filename)
    SaveFileText(filename, content)    
    fs.clear_content()
	log(`{filename} saved.`)
end

function fs.line(line)
    content = content .. line .. "\n"
end

return fs
