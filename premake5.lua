workspace "pixie"
    configurations { "Debug", "Test", "Stage", "Release" }
    platforms { "macos64", "linux64", "windows64" }

project "pixie"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetname "%{prj.name}"
    targetdir "bin/%{cfg.buildcfg}"
    entrypoint ("main()")
    files { "src/**.h", "src/**.cpp" }

    filter { "platforms:macos64" }
        system "macosx"
        architecture "x86_64"

    filter { "platforms:linux64" }
        system "linux"
        architecture "x86_64"

    filter { "platforms:windows64" }
        system "windows"
        architecture "x86_64"

    filter { "platforms:macos64", "configurations:Debug" }
        buildoptions { "-Wall", "-Wextra", "-fsanitize=undefined", "-fsanitize=address" }
        linkoptions { "-fsanitize=undefined", "-fsanitize=address" }
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter { "platforms:linux64", "configurations:Debug" }
        buildoptions { "-Wall", "-Wextra" }
        linkoptions { "-static" }
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter { "platforms:windows64", "configurations:Debug" }
        buildoptions { "-Wall", "-Wextra" }
        linkoptions { "-static" }
        flags { "FatalWarnings" }
        defines { "DEBUG" }
        symbols "On"

    filter{ "platforms:macos64", "configurations:Test" }
        files{ "src/**.h", "src/**.cpp", "test/**.h", "test/**.cpp" }
        removefiles{
            "src/main.cpp",
            "src/include/bench.h",
            "src/bench.cpp",
            "src/include/uci.h",
            "src/uci.cpp"
        }
        buildoptions{ "-Wall", "-Wextra" }
        flags{ "FatalWarnings", "LinkTimeOptimization" }
        defines{ "NDEBUG" }
        optimize "Speed"

    filter{ "platforms:linux64", "configurations:Test" }
        files{ "src/**.h", "src/**.cpp", "test/**.h", "test/**.cpp" }
        removefiles{
            "src/main.cpp",
            "src/include/bench.h",
            "src/bench.cpp",
            "src/include/uci.h",
            "src/uci.cpp"
        }
        buildoptions{ "-Wall", "-Wextra" }
        linkoptions { "-static" }
        flags{ "FatalWarnings", "LinkTimeOptimization" }
        defines{ "NDEBUG" }
        optimize "Speed"

    filter{ "platforms:windows64", "configurations:Test" }
        files{ "src/**.h", "src/**.cpp", "test/**.h", "test/**.cpp" }
        removefiles{
            "src/main.cpp",
            "src/include/bench.h",
            "src/bench.cpp",
            "src/include/uci.h",
            "src/uci.cpp"
        }
        buildoptions{ "-Wall", "-Wextra" }
        linkoptions { "-static" }
        flags{ "FatalWarnings", "LinkTimeOptimization" }
        defines{ "NDEBUG" }
        optimize "Speed"

    filter { "platforms:macos64", "configurations:Stage" }
        buildoptions { "-Wall", "-Wextra", "-march=native", "-fsanitize=undefined", "-fsanitize=address" }
        linkoptions { "-fsanitize=undefined", "-fsanitize=address" }
        flags { "FatalWarnings" }
        defines { "NDEBUG" }
        optimize "Speed"

    filter { "platforms:macos64", "configurations:Release" }
        buildoptions { "-Wall", "-Wextra", "-march=native" }
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "Speed"

    filter { "platforms:linux64", "configurations:Release" }
        buildoptions { "-Wall", "-Wextra", "-march=native" }
        linkoptions { "-static" }
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "Speed"

    filter { "platforms:windows64", "configurations:Release" }
        buildoptions { "-Wall", "-Wextra", "-march=native" }
        linkoptions { "-static" }
        flags { "FatalWarnings", "LinkTimeOptimization" }
        defines { "NDEBUG" }
        optimize "Speed"
