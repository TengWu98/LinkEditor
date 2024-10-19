include "Dependencies.lua"

workspace "MeshEditor"

    startproject "MeshEditor"

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
        include "lib/ImGui"
        include "lib/GLFW"
    group ""


project "MeshEditor"
    
    kind "ConsoleApp"

    language "C++"

    cppdialect "C++20"

    staticruntime "on"

    targetdir ("%{wks.location}/binaries/" .. outputdir .. "/%{prj.name}")

    objdir ("%{wks.location}/intermediate/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"

    pchsource "src/pch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",

        "%{IncludeDir.stb_image}/**.h",
        "%{IncludeDir.stb_image}/**.cpp",

        "%{IncludeDir.glm}/**.hpp",
        "%{IncludeDir.glm}/**.inl",

        "%{IncludeDir.ImGuizmo}/ImGuizmo.h",
		"%{IncludeDir.ImGuizmo}/ImGuizmo.cpp"
    }

    includedirs
    {
        "src",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.stb_image}",
    }

    links
    {
        "ImGui",
        "GLFW",
        "opengl32",
    }

    flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "MESH_EDITOR_PLATFORM_WINDOWS",
            "MESH_EDITOR_ENABLE_ASSERTS",
        }

    filter "configurations:Debug"
        defines
        {
            "MESH_EDITOR_DEBUG"
        }

        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines
        {
            "MESH_EDITOR_RELEASE"
        }

        runtime "Release"
        optimize "on"