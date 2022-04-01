include "./vendor/premake/premake_customization/solution_items.lua"
workspace "Hammer"
	architecture "x86_64"
	startproject "Hammer-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Hammer/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Hammer/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Hammer/vendor/imgui/"
IncludeDir["glm"] = "%{wks.location}/Hammer/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Hammer/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Hammer/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hammer/vendor/yaml-cpp/include"

group "Dependencies"
	include "vendor/premake"
	include "Hammer/vendor/GLFW"
	include "Hammer/vendor/Glad"
	include "Hammer/vendor/imgui"
	include "Hammer/vendor/yaml-cpp"
group ""

include "Hammer"
include "Hammer-Editor"
include "Sandbox"
