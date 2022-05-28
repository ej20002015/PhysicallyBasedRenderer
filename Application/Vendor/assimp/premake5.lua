project "assimp"
    language "C"
    staticruntime "on" -- Link CRT statically
    kind "StaticLib"
    systemversion "latest"

    targetdir(outputDirectory)
    objdir(intermediateDirectory)

    files
    {
        "code/**.cpp",
        "code/**.h",
        "include/**",
        "contrib/unzip/**",
        "contrib/openddlparser/code/**",
        "contrib/openddlparser/include/**"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "ASSIMP_BUILD_NO_C4D_IMPORTER",
        "ASSIMP_BUILD_NO_IFC_IMPORTER",
        "RAPIDJSON_HAS_STDSTRING",
        "OPENDDLPARSER_BUILD"
    }

    includedirs
    {
        "include",
        "code",
        "contrib/zlib",
        "contrib/unzip",
        "contrib/stb",
        "contrib/rapidjson/include",
        "contrib/rapidjson/include/rapidjson",
        "contrib",
        "contrib/pugixml/src",
        "contrib/openddlparser/include",
        ".",
    }

    defines
    {
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"