workspace "GameEngine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "MegaMario"
	location "MegaMario"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir.. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir.. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"vendor/IMGUI/include",
		"vendor/IMGUI-SFML/include",
		"vendor/SFML/include"
	}

	libdirs
	{
		"vendor/SFML/lib"
	}

	defines
	{
		"SFML_STATIC"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"
	
		defines
		{

		}

	filter "configurations:Debug"
		defines "GE_DEBUG"
		symbols "On"

		links
		{
			"sfml-graphics-s-d",
			"sfml-window-s-d",
			"sfml-system-s-d",
			"opengl32",
			"freetype",
			"winmm",
			"gdi32"
		}

	filter "configurations:Release"
		defines "GE_RELEASE"
		optimize "On"

		links
		{
			"sfml-graphics-s",
			"sfml-window-s",
			"sfml-system-s",
			"opengl32",
			"freetype",
			"winmm"
		}