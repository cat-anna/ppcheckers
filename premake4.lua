--FPS Engine premake4 build script generaor	
local Libs_common	= {	"freeimage",  "glfw3", "pthread"} 

local Libs_lin		= { "GL", "GLU", "X11", "Xxf86vm", "Xrandr", "Xi", "rt" }
local Libs_win		= { "opengl32", "glu32", "gdi32", }

solution "ppcheckers"
	configurations { "Release" }
	language "C++"
	
	links(Libs_common)
	includedirs { "src/", "src/**", "include/" }

	configuration "windows"
		links(Libs_win)
	configuration "linux"
		links(Libs_lin)
		defines "NO_WINDOWS"
		
	configuration "gmake"
		libdirs { "inclue/gcc" }
	configuration "vs2010"
		libdirs { "lib/vs" }
		
-- 	location "bin"
	
	project "ppcheckers"
		kind "ConsoleApp" 
		--kind "WindowedApp" 
		location "bin"
		files "src/**"
