#ifndef GameAppH
#define GameAppH

bool inline TestFlags(unsigned src, unsigned flags){
	return (src & flags) == flags;
}

void inline SetFlags(unsigned &src, unsigned flags, bool set){
	if(set)	src |= flags;
	else src &= ~flags;
}

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

#ifndef NO_WINDOWS 
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <c3DPoint.h>
#include <fonts.h>
#include "../cBoard.h"
#include "../cPlayerInterface.h"

typedef GLuint Texture;

enum {


	flag_BrainIsWorking			= 0x1000,

};

class cApplication {
	GLFWwindow *m_Window;
	volatile unsigned m_AppIsRunning;
	volatile unsigned m_Flags;
	unsigned m_GoalFPS;
	
	int m_Width, m_Height;
	float m_Aspect;
	int m_LastFPS;
	int m_FPSCounter;
	float m_LastRenderTime; //in ms, time needed to perform Render
	float m_LastMoveTime; //in ms, time needed to perform Move
	
	void ResizeOpenGL(int Width, int Height);

	void Render();
	void Initialize();
	void Finalize();

	void InitializeRenderLists();
	
	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_close_callback(GLFWwindow* window);
	static void glfw_focus_callback(GLFWwindow* window, int focus);
	static void glfw_mouse_callback(GLFWwindow *window, int button, int action, int mods);
	
	Texture m_BoardTex, m_PawnTex, m_QueenTex;
	cBitmapFont *bfHarrington;
	cOGLRenderList m_BoardRender; 
	cOGLRenderList m_RedPawn, m_WhitePawn; 
	cOGLRenderList m_RedQueen, m_WhiteQueen; 
	cOGLRenderList m_GrayField, m_GreenField, m_RedField;

	cBoard m_CurrentBoard;
	cBoard* m_RenderedBoard;

	cPlayerInterface *m_CurrentPlayer;
    cPlayerInterface *m_WhitePlayer;
    cPlayerInterface *m_BlackPlayer;

	cMoveVector m_PreviousMove;

	void RenderPawn(unsigned pawn, const c3DPoint &Where);
	void SafeListCall(cOGLRenderList &list, const c3DPoint &Where);

	cPlayerInterface* SetOppositePlayer();

	float m_WaitTime;
public:
	cApplication(int argc, char** argv);
	~cApplication();

	int Run();

	void SetFlags(unsigned Flags, bool state);
	GLuint LoadTexture(const char *TexFile);

	cPlayerInterface *CreatePlayer(bool IsCPU, PlayerType  ptype);
};

#endif
