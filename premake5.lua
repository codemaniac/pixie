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

    filter { "configurations:Debug" }
        system "macosx"
        architecture "x86_64"
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        system "macosx"
        architecture "x86_64"
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "On"
