workspace "Hammer"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Hammer/vendor/GLFW/include"
IncludeDir["Glad"] = "Hammer/vendor/Glad/include"
IncludeDir["ImGui"] = "Hammer/vendor/imgui/"
IncludeDir["glm"] = "Hammer/vendor/glm"
IncludeDir["stb_image"] = "Hammer/vendor/stb_image"

include "Hammer/vendor/GLFW"
include "Hammer/vendor/Glad"
include "Hammer/vendor/imgui"

project "Hammer"
	location "Hammer"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hmpch.h"
	pchsource "Hammer/src/hmpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}


	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}


	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		
		defines
		{
			"HM_PLATFORM_WINDOWS",
			"HM_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}


	filter "configurations:Debug"
		defines "HM_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HM_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HM_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Hammer/vendor/spdlog/include",
		"Hammer/src",
		"Hammer/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Hammer"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"HM_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HM_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HM_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HM_DIST"
		runtime "Release"
		optimize "on"