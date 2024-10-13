workspace "MeshEditor"

    startproject "MeshEditor"

    configurations
    {
        "Debug",
        "Release",
    }

    platforms
    {
        'x64'
    }

    filter "platforms:x64"
        architecture "x64"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    group "ThirdParty"
        include "lib/imgui"
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

    -- ImGui
    ImGuiDir = "lib/imgui"


    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "src",
        "%{ImGuiDir}",
        "%{ImGuiDir}/backends",
    }

    links
    {
        "ImGui",
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
        defines "MESH_EDITOR_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "MESH_EDITOR_RELEASE"
        runtime "Release"
        optimize "on"