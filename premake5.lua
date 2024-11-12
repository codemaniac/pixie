workspace "pixie"
    configurations { "Debug", "Test", "Release" }
    platforms { "macos64", "linux64" }

project "pixie"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetname "%{prj.name}"
    targetdir "bin/%{cfg.buildcfg}"
    entrypoint ("main()")
    files { "src/**.h", "src/**.cpp" }
    removefiles { "src/pypixie.cpp" }

    filter { "platforms:macos64" }
        system "macosx"
        architecture "x86_64"

    filter { "platforms:linux64" }
        system "linux"
        architecture "x86_64"

    filter { "configurations:Debug" }
        buildoptions { "-fsanitize=undefined" }
        linkoptions { "-fsanitize=undefined" }
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter { "configurations:Test" }
        buildoptions { "-fsanitize=undefined" }
        linkoptions { "-fsanitize=undefined" }
        flags { "FatalWarnings" }
        defines { "NDEBUG" }
        optimize "Speed"

    filter { "configurations:Release" }
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "Speed"
