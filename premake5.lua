workspace "pixie"
    configurations { "Debug", "UnitTest", "Test", "Release" }
    platforms { "macos64", "linux64", "windows64" }

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

    filter { "platforms:windows64" }
        system "windows"
        architecture "x86_64"

    filter { "configurations:Debug" }
        buildoptions { "-Wall", "-Wextra", "-fsanitize=undefined", "-fsanitize=address" }
        linkoptions { "-fsanitize=undefined", "-fsanitize=address" }
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter{"configurations:UnitTest"}
        files{ "src/**.h", "src/**.cpp", "test/**.h", "test/**.cpp" }
        removefiles{
            "src/main.cpp",
            "src/include/bench.h",
            "src/bench.cpp",
            "src/include/uci.h",
            "src/uci.cpp",
            "src/pypixie.cpp"
        }
        buildoptions{"-Wall", "-Wextra"}
        flags{"FatalWarnings"}
        defines{"NDEBUG"}
        optimize "Speed"

    filter { "configurations:Test" }
        buildoptions { "-Wall", "-Wextra", "-fsanitize=undefined", "-fsanitize=address" }
        linkoptions { "-fsanitize=undefined", "-fsanitize=address" }
        flags { "FatalWarnings" }
        defines { "NDEBUG" }
        optimize "Speed"

    filter { "configurations:Release" }
        buildoptions { "-Wall", "-Wextra"}
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "Speed"
