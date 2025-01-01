workspace "tejas"
    configurations { "Debug", "Test", "Release" }
    platforms { "macos64", "linux64", "windows64" }

project "tejas"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetname "%{prj.name}"
    targetdir "bin/%{cfg.buildcfg}"
    entrypoint ("main()")
    files { "src/**.h", "src/**.cpp" }
    pchheader "src/pch.h"
    pchsource "src/pch.cpp"
    includedirs { "src" }
    flags { "FatalWarnings", "LinkTimeOptimization" }
    buildoptions { "-Wall", "-Wextra", "-march=native", "-fsanitize=undefined", "-fsanitize=address" }
    linkoptions { "-static", "-fsanitize=undefined", "-fsanitize=address" }

    newoption {
        trigger = "enable-san",
        description = "Enable ASan and UBSan"
     }

    filter { "not options:enable-san" }
        removebuildoptions { "-fsanitize=undefined", "-fsanitize=address" }
        removelinkoptions { "-fsanitize=undefined", "-fsanitize=address" }

    filter { "platforms:macos64" }
        system "macosx"
        architecture "x86_64"
        removelinkoptions { "-static" }
        toolset ("clang")

    filter { "platforms:linux64" }
        system "linux"
        architecture "x86_64"
        toolset ("gcc")

    filter { "platforms:windows64" }
        system "windows"
        architecture "x86_64"
        toolset ("gcc")

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
        removeflags { "FatalWarnings", "LinkTimeOptimization" }

    filter { "configurations:Test" }
        files { "src/**.h", "src/**.cpp", "test/*.h", "test/*.cpp" }
        removefiles { "src/main.cpp" }
        includedirs { "src",  "test/lib/doctest" }
        removeflags { "FatalWarnings" }
        defines { "DEBUG", "TEST" }
        optimize "Speed"

    filter { "configurations:Release" }
        defines { "NDEBUG" }
        optimize "Speed"

newaction {
    trigger = "clean",
    description = "Remove all binaries and build folders.",
    execute = function()
        os.rmdir("./bin")
        os.rmdir("./obj")
    end
}
