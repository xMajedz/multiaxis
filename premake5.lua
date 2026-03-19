newoption {
	trigger = "location",
	description = "sets build directory.",
	default = "build",
}

workspace "tobas"
	language "C++"
	cppdialect "C++20"
	configurations { "Debug", "Release" }
	location (_OPTIONS["location"]) 

	includedirs {
		"vendor/luau/Compiler/include",
		"vendor/luau/VM/include",

		"vendor/raylib/src",
		"vendor/raygui/src",

		"vendor/ode/include",

		--"vendor/enet/include",

		"sources",
	}

	links {
		"luaucompiler", "luauast", "luauvm",
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

	filter { "configurations:Debug" }
	defines { "DEBUG" }
	symbols "On"
	
	filter { "configurations:Release" }
	defines { "NDEBUG" }
	optimize "On"

	filter { "system:Windows" }
	libdirs { "lib/windows" }

	filter { "not system:Windows" }
	libdirs { "lib/linux" }

project "multiaxis_sp"
	kind "WindowedApp"
	files { "sources/tobas_sp.cpp" }
	defines { "OFFLINE" }

	filter { "system:Windows" }
	links {"winmm", "gdi32", "opengl32"}
	defines{"_WIN32"}
--[[
project "tobas"
	kind "WindowedApp"
	files {
		"sources/tobas.cpp",
		"sources/netcode_common.h", "sources/netcode_common.cpp",
		"sources/netcode_client.h", "sources/netcode_client.cpp",
		"sources/netcode_server.h", "sources/netcode_server.cpp",
	}

	filter { "system:Windows" }
	links {"winmm", "gdi32", "opengl32"}
        links { "kernel32", "ws2_32" }
	defines{"_WIN32"}

project "tobas_server"
	kind "ConsoleApp"
	files {
		"sources/tobas_server.cpp",
		"sources/netcode_common.h", "sources/netcode_common.cpp",
		"sources/netcode_client.h", "sources/netcode_client.cpp",
		"sources/netcode_server.h", "sources/netcode_server.cpp",
	}

	filter { "system:Windows" }
	links {"winmm", "gdi32", "opengl32"}
        links { "kernel32", "ws2_32" }
	defines{"_WIN32"}
]]