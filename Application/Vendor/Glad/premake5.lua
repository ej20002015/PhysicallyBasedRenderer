project "Glad"

    language "C"
    staticruntime "on" -- Link CRT statically
    kind "StaticLib"
    systemversion "latest"

    targetdir(outputDirectory)
    objdir(intermediateDirectory)

    files
    {
        "src/glad.c",
        "include/glad/glad.h",
        "include/KHR/khrplatform.h"
    }

    includedirs
    {
        "include"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"