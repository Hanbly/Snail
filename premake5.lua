workspace "Snail"

    architecture "x64"
    configurations { 
        "Debug", 
        "Release", 
        "Dist"
    }

    startproject "SnailEditor" -- 启动项目设置

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
IncludeDirs["Assimp"] = "Snail/vendor/Assimp/include"
IncludeDirs["EnTT"] = "Snail/vendor/EnTT/include"
IncludeDirs["yaml"] = "Snail/vendor/yaml-cpp/include"
IncludeDirs["BoostUuid"] = "Snail/vendor/boost-uuid/include"
IncludeDirs["ImGuiFileDialog"] = "Snail/vendor/ImGuiFileDialog"

-- 引入需要编译项目（该项目需要编译，所以引入premake配置文件）
-- 实际引入的是项目的premake5.lua文件
include "Snail/vendor/GLFW"
include "Snail/vendor/GLAD"
include "Snail/vendor/Imgui"
include "Snail/vendor/Assimp"
include "Snail/vendor/yaml-cpp"


project "Snail"
    location "Snail"
    kind "StaticLib"
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
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/ImGuiFileDialog/ImGuiFileDialog.h",
        "%{prj.name}/vendor/ImGuiFileDialog/ImGuiFileDialog.cpp"
    }

    includedirs {
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.GLAD}",
        "%{IncludeDirs.GLM}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.stb_image}",
        "%{IncludeDirs.Assimp}",
        "%{IncludeDirs.EnTT}",
        "%{IncludeDirs.yaml}",
        "%{IncludeDirs.BoostUuid}",
        "%{IncludeDirs.ImGuiFileDialog}",
        "%{prj.name}/src"
    }

    links { 
        "GLFW",
        "opengl32.lib",
        "GLAD",
        "Imgui",
        "Assimp",
        "yaml-cpp"
    }

    filter "system:windows"

        defines {
            "_CRT_SECURE_NO_WARNINGS", -- 移除C库安全警告
            "IMGUI_IMPL_OPENGL_LOADER_CUSTOM", -- 禁止ImGui重定义OpenGL宏
            "YAML_CPP_STATIC_DEFINE", -- 使用yaml的静态库
            "USE_STD_FILESYSTEM", -- ImGuiFileDialog 宏表示使用标准的 filesystem 而不是旧的 dirent.h
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


project "SnailEditor"
    location "SnailEditor"
    kind "ConsoleApp"
    language "C++"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}/")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}/")

    files {
        "%{prj.name}/src/**.h", 
        "%{prj.name}/src/**.cpp",
    }

    includedirs {
        "%{IncludeDirs.GLFW}",
        "%{IncludeDirs.GLAD}",
        "%{IncludeDirs.GLM}",
        "%{IncludeDirs.ImGui}",
        "%{IncludeDirs.spdlog}",
        "%{IncludeDirs.Assimp}",
        "%{IncludeDirs.EnTT}",
        "%{IncludeDirs.yaml}",
        "%{IncludeDirs.BoostUuid}",
        "%{IncludeDirs.ImGuiFileDialog}",
        "%{prj.name}/src",
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


project "Example"
    location "Example"
    kind "ConsoleApp"
    language "C++"
    staticruntime "off"

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
        "%{IncludeDirs.Assimp}",
        "%{IncludeDirs.EnTT}",
        "%{IncludeDirs.yaml}",
        "%{IncludeDirs.BoostUuid}",
        "%{IncludeDirs.ImGuiFileDialog}",
        "%{prj.name}/src",
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
