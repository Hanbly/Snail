workspace "Snail"

    architecture "x64"
    configurations { 
        "Debug", 
        "Release", 
        "Dist"
    }

    startproject "Example" -- 启动项目设置

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        buildoptions { "/utf-8" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


-- 引入子项目include表
IncludeDirs = {}
IncludeDirs["GLFW"] = "Snail/vendor/GLFW/include"
IncludeDirs["GLAD"] = "Snail/vendor/GLAD/include"
IncludeDirs["GLM"] = "Snail/vendor/GLM"
IncludeDirs["ImGui"] = "Snail/vendor/Imgui"
IncludeDirs["spdlog"] = "Snail/vendor/spdlog/include"
IncludeDirs["stb_image"] = "Snail/vendor/stb_image"

-- 引入GLFW项目（该项目需要编译，所以引入premake配置文件）
-- 实际引入的是GLFW项目的premake5.lua文件
include "Snail/vendor/GLFW"
include "Snail/vendor/GLAD"
include "Snail/vendor/Imgui"


project "Snail"
    location "Snail"
    kind "StaticLib"
    -- kind "SharedLib"
    language "C++"
    staticruntime "off"

    pchheader "SNLpch.h"
    pchsource "%{prj.name}/src/SNLpch.cpp"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}/")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp"
    }

    includedirs {
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.GLAD}",
        "%{IncludeDirs.GLM}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.stb_image}",
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
            "_CRT_SECURE_NO_WARNINGS", -- 移除C库安全警告
            "IMGUI_IMPL_OPENGL_LOADER_CUSTOM", -- 禁止ImGui重定义OpenGL宏
            "SNL_PLATFORM_WINDOWS",
            "SNL_BUILD_SLL"
            -- "SNL_BUILD_DLL"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        defines {
            "SNL_DEBUG",
            "SNL_ENABLED_ASSERTS",
            "SNL_PROFILING"
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
        "%{IncludeDirs.GLM}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.spdlog}",
        "Snail/src"
    }

    links { "Snail" }

    filter { "system:windows" }

        defines {
            "SNL_PLATFORM_WINDOWS"
        }

    filter { "configurations:Debug" }
        runtime "Debug"
        symbols "On"
        defines { 
            "SNL_DEBUG",
            "SNL_ENABLED_ASSERTS",
            "SNL_PROFILING" 
        }

    filter { "configurations:Release" }
        defines { "SNL_RELEASE" }
        runtime "Release"
        optimize "On"

    filter { "configurations:Dist" }
        defines { "SNL_DIST" }
        optimize "On"