project "LinkEditor"
    
    kind "ConsoleApp"

    language "C++"

    cppdialect "C++17"

    staticruntime "off"

    targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")

    objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"

    pchsource "Source/pch.cpp"

    -- Asset
    assetdir = "%{wks.location}/Asset"

    -- Shader
    shaderdir = "%{wks.location}/Shaders"

    files
    {
        "Source/**.h",
        "Source/**.cpp",

        "%{IncludeDir.stb_image}/**.h",
        "%{IncludeDir.stb_image}/**.cpp",

        "%{IncludeDir.glm}/**.hpp",
        "%{IncludeDir.glm}/**.inl",

        "%{IncludeDir.ImGuizmo}/ImGuizmo.h",
		"%{IncludeDir.ImGuizmo}/ImGuizmo.cpp",

        "Shaders/GLSL/**.glsl"
    }

    includedirs
    {
        "Source",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.OpenMesh}",
        "%{IncludeDir.nfd}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.Eigen}",
    }

    links
    {
        "ImGui",
        "GLFW",
        "Glad",
        "opengl32",
        "ThirdParty/nfd/lib/nfd.lib"
    }

    flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "LINK_EDITOR_PLATFORM_WINDOWS",
            "LINK_EDITOR_ENABLE_ASSERTS",

            "_USE_MATH_DEFINES", -- OpenMesh
        }

    filter "configurations:Debug"
        defines
        {
            "LINK_EDITOR_DEBUG"
        }

        links
        {
            "ThirdParty/OpenMesh/lib/OpenMeshCored.lib",
            "ThirdParty/OpenMesh/lib/OpenMeshToolsd.lib",
        }

        postbuildcommands
        {
            -- '{COPYDIR} "%{shaderdir}" "%{wks.location}/Binaries/%{outputdir}/%{prj.name}/Shaders"',
            -- '{COPYDIR} "%{assetdir}" "%{wks.location}/Binaries/%{outputdir}/%{prj.name}/Asset"',
        }

        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines
        {
            "LINK_EDITOR_RELEASE"
        }

        links
        {
            "ThirdParty/OpenMesh/lib/OpenMeshCore.lib",
            "ThirdParty/OpenMesh/lib/OpenMeshTools.lib",
        }

        postbuildcommands
        {
            -- '{COPYDIR} "%{shaderdir}" "%{wks.location}/binaries/%{outputdir}/%{prj.name}/Shader"',
            -- '{COPYDIR} "%{assetdir}" "%{wks.location}/binaries/%{outputdir}/%{prj.name}/Asset"',
            -- '{COPYDIR} "%{wks.location}/binaries/%{outputdir}/%{prj.name}" "%{wks.location}/MeshEditorRelease"'
        }

        runtime "Release"
        optimize "on"