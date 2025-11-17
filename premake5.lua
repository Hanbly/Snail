workspace "Snail"

    architecture "x64"
    configurations { 
        "Debug", 
        "Release", 
        "Dist"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        buildoptions { "/utf-8" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Snail"
    location "Snail"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}/")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/vendor/spdlog/include",
        "%{prj.name}/src"
    }

    filter "system:windows"

        defines {
            "SNL_PLATFORM_WINDOWS",
            "SNL_BUILD_DLL"
        }

    filter "configurations:Debug"
        defines { "SNL_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "SNL_RELEASE" }
        optimize "On"

    filter "configurations:Dist"
        defines { "SNL_DIST" }
        optimize "On"


project "Example"
    location "Example"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}/")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

    files {
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "Snail/vendor/spdlog/include",
        "Snail/src"
    }

    links { "Snail" }
    dependson { "Snail" }

    filter { "system:windows" }

        defines {
            "SNL_PLATFORM_WINDOWS"
        }

        postbuildcommands {
            -- 将 Snail.dll 复制到 Example.exe 所在的目录
            -- 使用 %{cfg.targetdir} 可以自动获取当前项目的输出目录
            ("{COPYFILE} ../bin/" .. outputdir .. "/Snail/Snail.dll %{cfg.targetdir}")
        }

    filter { "configurations:Debug" }
        defines { "SNL_DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        defines { "SNL_RELEASE" }
        optimize "On"

    filter { "configurations:Dist" }
        defines { "SNL_DIST" }
        optimize "On"