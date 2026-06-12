newoption {
    trigger = "location",
    description = "sets build directory.",
}

newoption {
    trigger = "version",
    description = "sets build version.",
}

local loc = _TARGET_OS .. "-build"
if _OPTIONS["os"] then
   loc = (_OPTIONS["os"] .. "-build") or loc
end
loc = _OPTIONS["location"] or loc

if _OPTIONS["version"] then
   local game_version_h = io.open("./sources/game_version.h", "w")
   game_version_h:write("#define GAME_VERSION \"" .. _OPTIONS["version"] .. "\"")
   game_version_h:close()
end

workspace "mulitaxis"
	language "C++"
	configurations { "release", "debug" }
	location (loc)
	targetdir (loc)

	includedirs {
		"vendor/luau/Compiler/include",
		"vendor/luau/VM/include",

		"vendor/raylib/src",
		"vendor/raygui/src",

		"vendor/ode/include",

		"sources",
	}

	links {
		"luaucompiler", "luauast", "luauvm",
		
		"luaucommon", "luaucodegen",

		"raylib",
		
		"ode",
	}

	files {
		"sources/luau.h", "sources/luau.cpp",

		"sources/api.h", "sources/api.cpp",
		"sources/api_net.cpp",
		"sources/api_game.cpp",
		"sources/api_raylib.cpp",
		"sources/api_raygui.cpp",
		"sources/api_raymath.cpp",

		"sources/game.h", "sources/game.cpp",
		"sources/camera.h", "sources/camera.cpp",
		"sources/player.h", "sources/player.cpp",
		"sources/body.h", "sources/body.cpp",

		"sources/mem.h", "sources/mem.cpp",
	}

	filter { "configurations:debug" }
	defines { "DEBUG" }
	symbols "On"
	
	filter { "configurations:release" }
	defines { "NDEBUG" }
	optimize "On"

	filter { "system:windows" }
	libdirs { "windows-lib" }

	filter { "system:linux" }
	libdirs { "linux-lib" }

project "multiaxis"
	kind "WindowedApp"
	files { "sources/multiaxis.cpp" }

	defines { "OFFLINE" }

	filter { "system:linux" }
	links { "X11" }

	filter { "system:windows" }
	links { "winmm", "gdi32", "opengl32" }
	defines { "_WIN32" }
