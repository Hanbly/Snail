workspace "Snail"

    architecture "x64"
    configurations { 
        "Debug", 
        "Release", 
        "Dist"
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        buildoptions { "/utf-8" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


-- 引入子项目include表
IncludeDirs = {}
IncludeDirs["GLFW"] = "Snail/vendor/GLFW/include"
IncludeDirs["GLAD"] = "Snail/vendor/GLAD/include"
IncludeDirs["ImGui"] = "Snail/vendor/Imgui"
IncludeDirs["spdlog"] = "Snail/vendor/spdlog/include"

-- 引入GLFW项目（该项目需要编译，所以引入premake配置文件）
-- 实际引入的是GLFW项目的premake5.lua文件
include "Snail/vendor/GLFW"
include "Snail/vendor/GLAD"
include "Snail/vendor/Imgui"


project "Snail"
    location "Snail"
    kind "SharedLib"
    language "C++"
    staticruntime "off"

    pchheader "SNLpch.h"
    pchsource "%{prj.name}/src/SNLpch.cpp"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}/")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.GLAD}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.spdlog}",
        "%{prj.name}/src"
    }

    links { 
        "GLFW",
        "opengl32.lib",
        "GLAD",
        "Imgui"
    }

    filter "system:windows"

        defines {
            "SNL_PLATFORM_WINDOWS",
            "SNL_BUILD_DLL"
        }

    filter "configurations:Debug"
        defines { "SNL_DEBUG" }
        runtime "Debug"
        symbols "On"
        defines {
            "SNL_ENABLED_ASSERTS"
        }

    filter "configurations:Release"
        defines { "SNL_RELEASE" }
        runtime "Release"
        optimize "On"

    filter "configurations:Dist"
        defines { "SNL_DIST" }
        optimize "On"


project "Example"
    location "Example"
    kind "ConsoleApp"
    language "C++"
    staticruntime "off"

    pchheader "SNLpch.h"
    pchsource "%{prj.name}/src/SNLpch.cpp"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}/")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

    files {
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.GLAD}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.spdlog}",
        "Snail/src"
    }

    links { "Snail" }

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
        runtime "Debug"
        symbols "On"

    filter { "configurations:Release" }
        defines { "SNL_RELEASE" }
        runtime "Release"
        optimize "On"

    filter { "configurations:Dist" }
        defines { "SNL_DIST" }
        optimize "On"