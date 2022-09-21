project "Application"

    language "C++"
    cppdialect "C++17"
    staticruntime "on" -- Link CRT statically
    kind "ConsoleApp"
    systemversion "latest"

    targetdir(outputDirectory)
    objdir(intermediateDirectory)

    -- Use PCH to reduce compilation time and number of includes
    pchheader "PCH.h"
    pchsource "src/PCH.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "src",
        "Vendor/Glad/include",
        "Vendor/GLFW/include/GLFW",
        "Vendor/GLM",
        "Vendor/spdlog/include",
        "Vendor/stb_image",
        "Vendor/assimp/include"
    }

    links
    {
        "Glad",
        "GLFW",
        "assimp"
    }

    filter "system:windows"
        defines
        {
            "PBR_WINDOWS"
        }

    filter "system:linux"
        defines
        {
            "PBR_LINUX"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

        defines
        {
            "GLFW_INCLUDE_NONE",
            "PBR_DEBUG",
            "SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE",
            "SPDLOG_ACTIVE_LEVEL_SOURCE=spdlog::level::trace"
        }
    
    filter {"configurations:Debug", "system:windows"}
        links
        {
            "Vendor/assimp/zlib/bin/Windows/Debug/zlibstaticd.lib"
        }
    
    filter {"configurations:Debug", "system:linux"}
        links
        {
            "zlibstaticd:shared"
        }

        libdirs
        {
            "Vendor/assimp/zlib/bin/Linux/Debug/"
        }

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

        defines
        {
            "GLFW_INCLUDE_NONE",
            "PBR_RELEASE",
            "SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO",
            "SPDLOG_ACTIVE_LEVEL_SOURCE=spdlog::level::info"
        }
    
    filter {"configurations:Release", "system:windows"}
        links
        {
            "Vendor/assimp/zlib/bin/Windows/Release/zlibstatic.lib"
        }

    filter {"configurations:Release", "system:linux"}
        links
        {
            "zlibstatic:shared"
        }

        libdirs
        {
            "Vendor/assimp/zlib/bin/Linux/Release/"
        }