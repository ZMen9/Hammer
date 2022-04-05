include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"
workspace "Hammer"
	architecture "x86_64"
	startproject "Hammer-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "Hammer/vendor/GLFW"
	include "Hammer/vendor/Glad"
	include "Hammer/vendor/imgui"
	include "Hammer/vendor/Box2D"
	include "Hammer/vendor/yaml-cpp"
group ""

include "Hammer"
include "Hammer-Editor"
include "Sandbox"