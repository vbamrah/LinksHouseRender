solution "scene"
configurations {"Debug", "Release"}


project "scene"
kind "consoleapp"
language "c++"
cppdialect "c++17"
architecture "x86_64"
	
outputdir  = "%{cfg.longname}"

includedirs{
    "vendor",
    "vendor/GLAD/include",
    "src",
  }
  
files{
    "src/**.h", "src/**.cpp",
}

libdirs{
  "vendor/GLAD/%{cfg.longname}", "vendor/GLFW/%{cfg.longname}"  
  }

 
links { "gdi32", "kernel32", "shell32", "glu32", "Glad.a", "GLFW.a", "user32"}


targetdir("bin/%{cfg.longname}")
objdir("obj/%{cfg.longname}")

filter "configurations:Debug"
	runtime "Debug"
	symbols "on"
  
  defines{ "MD_DEBUG", }

filter "configurations:Release"
	runtime "Release"
	optimize "on"
  
  defines{ 
    "MD_RELEASE",
    "MD_WINDOWS",
    "MD_OPENGL",
    "MD_PLATFORM MD_WINDOWS" }

filter "configurations:All"
