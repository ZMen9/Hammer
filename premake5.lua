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

include "Hammer/vendor/GLFW"
include "Hammer/vendor/Glad"
include "Hammer/vendor/imgui"

project "Hammer"
	location "Hammer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hmpch.h"
	pchsource "Hammer/src/hmpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"

	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
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

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "HM_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HM_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HM_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		symbols "On"

	filter "configurations:Release"
		defines "HM_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HM_DIST"
		runtime "Release"
		optimize "On"