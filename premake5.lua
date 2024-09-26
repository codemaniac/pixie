workspace "pixie"
    configurations { "Debug", "Release" }
    platforms { "macos", "linux64" }
    version = "0.1.0-beta"

    function getTargetName()
        local os_name = os.target()
        local arch_name = "x86_64"
        return "%{prj.name}_" .. os_name .. "_" .. arch_name .. "_v" .. version
    end

project "pixie"
    kind "ConsoleApp"
    language "C"
    targetname (getTargetName())
    targetdir "bin/%{cfg.buildcfg}"
    entrypoint ("main()")
    includedirs { "src/include" }
    files { "src/**.h", "src/**.c" }
    removefiles { "src/pypixie.c" }

    filter { "platforms:macos" }
        system "macosx"
        architecture "x86_64"

    filter { "platforms:linux64" }
        system "linux"
        architecture "x86_64"

    filter { "configurations:Debug" }
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "speed"
