workspace "PBR"

    architecture "x86_64"
    startproject "Application"

    configurations
    {
        "Debug",
        "Release"
    }

    outputDirectory =       "bin/%{cfg.buildcfg}/%{prj.name}"
    intermediateDirectory = "bin-int/%{cfg.buildcfg}/%{prj.name}"

    include "Application"
    -- Place dependency projects in a virtual folder
    group "Dependencies"
        include "Application/Vendor/Glad"
        include "Application/Vendor/GLFW"
        include "Application/vendor/assimp"
    group ""