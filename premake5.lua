workspace "pixie"
    configurations { "Debug", "Release" }
    platforms { "MacOS64", "Win64" }

project "pixie"
    kind "ConsoleApp"
    language "C"
    targetname ("pixie")
    targetdir "bin/%{cfg.buildcfg}"
    entrypoint ("main()")
    includedirs { "src/include" }
    files { "src/**.h", "src/**.c" }
    removefiles { "src/pypixie.c" }

    filter { "platforms:MacOS64" }
        system "macosx"
        architecture "x86_64"

    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

    filter "configurations:Debug"
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "On"
