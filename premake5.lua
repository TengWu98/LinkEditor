include "Dependencies.lua"

workspace "LinkEditor"

    startproject "LinkEditor"

    configurations
    {
        "Debug",
        "Release",
    }

    platforms
    {
        'Win64'
    }

    flags
	{
		"MultiProcessorCompile"
	}

    filter "platforms:Win64"
        system "windows"
        architecture "x64"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    group "Dependencies"
        include "LinkEditor/ThirdParty/ImGui"
        include "LinkEditor/ThirdParty/GLFW"
        include "LinkEditor/ThirdParty/Glad"
    group ""

    group "Core"
        include "LinkEditor"
    group ""