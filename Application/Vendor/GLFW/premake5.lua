project "GLFW"
    language "C"
    staticruntime "on" -- Link CRT statically
    kind "StaticLib"
    systemversion "latest"

    targetdir(outputDirectory)
    objdir(intermediateDirectory)

    files
    {
        "include/GLFW/glfw3.h",
		"include/GLFW/glfw3native.h",
		"src/internal.h",
		"src/mappings.h",
        "src/null_joystick.h",
        "src/null_platform.h",
        "src/platform.h",
        "src/context.c",
        "src/egl_context.c",
		"src/init.c",
		"src/input.c",
		"src/monitor.c",
        "src/null_init.c",
        "src/null_joystick.c",
        "src/null_monitor.c",
        "src/null_window.c",
        "src/osmesa_context.c",
        "src/platform.c",
		"src/vulkan.c",
		"src/window.c",

        -- Windows specific files

        "src/win32_joystick.h",
        "src/win32_platform.h",
        "src/win32_init.c",
        "src/win32_joystick.c",
        "src/win32_monitor.c",
        "src/win32_window.c",
        "src/wgl_context.c",
        "src/win32_time.h",
        "src/win32_thread.h",
        "src/win32_module.c",
        "src/win32_time.c",
        "src/win32_thread.c"
    }

    defines
    {
        "_GLFW_WIN32",
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"