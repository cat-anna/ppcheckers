#include "App.h"
#include <D2Math.h>
#include <ctime>
#include <FreeImage.h>
#include <cassert>
#include <fstream>

#include "../cInputPlayer.h"
#include "../cBrain.h"

static void glfw_error_callback(int error, const char* description){
	cerr << "GLFW error: code=" << error << " descr='" << description << "'";
}

cApplication::cApplication(int argc, char** argv): 
		m_Window(), m_AppIsRunning(0), m_Flags(), m_CurrentBoard(){
	srand(time(NULL));
	m_LastFPS = 30;//min goal
	m_GoalFPS = 30;
	m_FPSCounter = 0;
	m_LastMoveTime = m_LastRenderTime = 0;
	m_Flags = 0;

	m_Width = 800;
	m_Height = 600;
	m_Aspect = m_Width / (float)m_Height;

	//sprawdzenie parametrów graczy
	bool WhiteCPU = argc > 1 ? argv[1][0] == 'k' : false;
	bool RedCPU = argc > 2 ? argv[2][0] != 'c' : true;
	//bo mi siê nie chce pisaæ tego lepiej

	m_WhitePlayer = CreatePlayer(WhiteCPU, PlayerWhite);
    m_BlackPlayer = CreatePlayer(RedCPU, PlayerBlack);

	m_CurrentPlayer = 0;
	m_WaitTime = 0;
}

cApplication::~cApplication(){
    delete m_BlackPlayer;
}

cPlayerInterface* cApplication::CreatePlayer(bool IsCPU, PlayerType ptype) {
	if(IsCPU)
		return new cBrain(ptype);
	else
		return new cInputPlayer(ptype);
}

cPlayerInterface* cApplication::SetOppositePlayer() {
	if(m_CurrentPlayer == m_WhitePlayer) m_CurrentPlayer = m_BlackPlayer;
	else m_CurrentPlayer = m_WhitePlayer;
	m_RenderedBoard = m_CurrentPlayer->UseMyBoard();
	if(!m_RenderedBoard) m_RenderedBoard = &m_CurrentBoard;
	return m_CurrentPlayer;
}

void cApplication::SetFlags(unsigned Flags, bool Enable){
	if(Enable) m_Flags |= Flags;
	else m_Flags &= ~Flags;
}

c3DPoint CurrPlayerInfoPos;

c3DPoint FieldPos(int id){
	c3DPoint r;
	r.x += BoardOffset.x + ((id % 8) + 0.5f) * FieldSize.x;
	r.y += BoardOffset.y + ((id / 8) + 0.5f) * FieldSize.y;
	return r;
}

void cApplication::Initialize(){	
	glfwSetErrorCallback(&glfw_error_callback);
	if(!glfwInit())	throw "!glfw";
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	m_Window = glfwCreateWindow(m_Width, m_Height, "Warcaby", 0, 0);
	if(!m_Window) throw "!Window";
	glfwMakeContextCurrent(m_Window);
	
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetKeyCallback(m_Window, glfw_key_callback);
	glfwSetWindowCloseCallback(m_Window, glfw_close_callback);
	glfwSetMouseButtonCallback(m_Window, glfw_mouse_callback);
	glfwSetWindowFocusCallback(m_Window, glfw_focus_callback);
	
	glClearColor(0.0, 0.0, 0.0, 0.0); 	  
	glShadeModel(GL_SMOOTH);               
//	glClearDepth(1.0);                       
	//glDisable(GL_DEPTH_TEST);                
//	glDepthFunc(GL_LEQUAL);		           
	glEnable(GL_TEXTURE_2D);         
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glAlphaFunc(GL_GREATER, 0.4f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);	
	
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_Width, m_Height, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);	
	
	m_BoardTex = LoadTexture("data/plansza.jpg");
	m_PawnTex = LoadTexture("data/pionek.png");
	m_QueenTex = LoadTexture("data/queen.png");
	bfHarrington = new cBitmapFont("data/Harrington.bfd", "data/Harrington.tga");
	
	InitializeRenderLists();
}

void cApplication::Finalize() {
	delete bfHarrington;
	glfwDestroyWindow(m_Window);
	glfwTerminate();	
}

void cApplication::Render(){
	++m_FPSCounter;

	//process current player
	double Mx, My;
	glfwGetCursorPos(m_Window, &Mx, &My);
	c3DPoint mp(static_cast<float>(Mx), static_cast<float>(My), 0);
	mp -= BoardOffset;
	mp.x /= FieldSize.x;
	mp.y /= FieldSize.y;
	cBoardCord cord(mp.x, mp.y);
	bool RenderFocusField = true;

	if(!m_CurrentPlayer) {
		SetOppositePlayer();
		m_CurrentPlayer->UpdateBoard(m_PreviousMove, m_CurrentBoard);
		m_CurrentPlayer->Work();
	} else {
		RenderFocusField = m_CurrentPlayer->PushCursorPos(cord);
		if(m_CurrentPlayer->IsMoveReady()){
			m_WaitTime = 3;
			if(m_CurrentPlayer->IsAIPlayer())
				m_WaitTime = glfwGetTime() + GameSettings.WaitTimeBeforeAIPlayer;
			const sMoveInfo &mi = m_CurrentPlayer->PeekLastMove();
			m_PreviousMove = mi.MovesFromParent;
			m_CurrentBoard = mi.Board;
			SetOppositePlayer();
			m_CurrentPlayer->UpdateBoard(m_PreviousMove, m_CurrentBoard);
		}
	}

	if(m_WaitTime > 0 && m_WaitTime < glfwGetTime()){
		
		m_CurrentPlayer->Work();
		m_WaitTime = -1;
	}

	glLoadIdentity();
	m_BoardRender.Call();
	if(m_CurrentPlayer == m_BlackPlayer) SafeListCall(m_RedQueen, CurrPlayerInfoPos);
	else SafeListCall(m_WhiteQueen, CurrPlayerInfoPos);


	glDisable(GL_TEXTURE_2D);
	glColor4f(0.5, 0.5, 0.5, 1.0);
	glBegin(GL_LINE_STRIP);
	for(auto it = m_PreviousMove.begin(), jt = m_PreviousMove.end(); it != jt; ++it){
		c3DPoint pos = FieldPos(*it);
		glVertex2f(pos.x, pos.y);
	}
	glEnd();
	const sMoveInfo &mi = m_CurrentPlayer->PeekLastMove();
	glColor4f(0.0, 0.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for(auto it = mi.MovesFromParent.begin(), jt = mi.MovesFromParent.end(); it != jt; ++it){
		c3DPoint pos = FieldPos(*it);
		glVertex2f(pos.x, pos.y);
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);

	for(int i = 0; i < 64; ++i) {
		cField &f = m_RenderedBoard->at(i);
		//if(!f) continue;
		if(f.GetStatus() == cField::fsPickedUp) continue;

		cOGLRenderList *lst = 0;
		switch(f.GetStatus()){
			case cField::fsPickedUp:
			case cField::fsNone:
				break;
			case cField::fsRedSelect:
				lst = &m_RedField;
				break;
			case cField::fsGreenSelect:
				lst = &m_GreenField;
				break;
			case cField::fsGraySelect:
				lst = &m_GrayField;
				break;
			case cField::fsPossibleMove:
				f = 0;
				continue;
		};
		c3DPoint pos = FieldPos(i);
		if(f) RenderPawn(f, pos);
		if(lst) SafeListCall(*lst, pos);
		f.SetStatus(cField::fsNone);
	}

	if(RenderFocusField){
		c3DPoint pos = FieldPos(cord.Linear());
		if(m_RenderedBoard->at(cord).empty())
			SafeListCall(m_GrayField, pos);
		else
			SafeListCall(m_RedField, pos);
	}

	glfwSwapBuffers(m_Window);
}

void cApplication::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action == GLFW_REPEAT) return;
	cApplication* app = ((cApplication*)glfwGetWindowUserPointer(window));
	bool press = action == GLFW_PRESS;
	switch (key){
	case GLFW_KEY_ESCAPE:
		if(!press) return;
		app->m_AppIsRunning = 0;
		return;
	}
}

void cApplication::glfw_close_callback(GLFWwindow* window){
	cApplication* app = ((cApplication*)glfwGetWindowUserPointer(window));
	app->m_AppIsRunning = 0;
}

void cApplication::glfw_mouse_callback(GLFWwindow *window, int button, int action, int mods){
	if(button != GLFW_MOUSE_BUTTON_LEFT) return;
	cApplication* app = ((cApplication*)glfwGetWindowUserPointer(window));
	if(!app->m_CurrentPlayer) return;
	if(action == GLFW_PRESS){
		app->m_Flags |= flag_MouseLeftDown | flag_MouseLeftPressed;
	} else {
		(app->m_Flags &= ~flag_MouseLeft) |= flag_MouseLeftReleased;
	}

	app->m_CurrentPlayer->SetMouseState(app->m_Flags & flag_MouseLeft);
}

void cApplication::glfw_focus_callback(GLFWwindow* window, int focus) {
	cApplication* app = ((cApplication*)glfwGetWindowUserPointer(window));
}

int cApplication::Run(){
	Initialize();
	m_AppIsRunning = 1;
	double flt = glfwGetTime();
	double rlt = flt;
	double fw = 1 / static_cast<float>(m_GoalFPS);
	int ldfps = 0;
	while (m_AppIsRunning) {
		glfwPollEvents();

		double ct = glfwGetTime();
		if(ct - flt >= 1.0){
			flt = ct;
			m_LastFPS = m_FPSCounter;
			m_FPSCounter = 0;
			int dfps = m_GoalFPS - m_LastFPS;
			int dabs = abs(dfps);
			if(dabs > 5 && ldfps < 6) dfps = 0;
			fw -= (dfps * 0.0001f) * ((ldfps == 0 && dabs == 1)? 0.1f : 1.0f);
			if(fw < 0) fw = 0;
			ldfps = dabs;			
		}
		if(ct - rlt >= fw){
			rlt = ct;
			Render();
		}
	}
	Finalize();
	return 0;
}

//------------------------------------------------------------------------------------------
//Funkcje niezbêdne ale irytuj¹ce i d³ugie
//------------------------------------------------------------------------------------------

void cApplication::RenderPawn(unsigned pawn, const c3DPoint &Where) {
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(Where.x, Where.y, 0);
	switch(pawn){
	case ptWhitePawn:		
		m_WhitePawn.Call();
		break;
	case ptWhiteQueen:
		m_WhiteQueen.Call();
		break;
	case ptBlackPawn:
		m_RedPawn.Call();
		break;
	case ptBlackQueen:
		m_RedQueen.Call();
		break;
	}
	glPopMatrix();
}

void cApplication::SafeListCall(cOGLRenderList &list, const c3DPoint &Where) {
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(Where.x, Where.y, 0);
	list.Call();
	glPopMatrix();
}

void cApplication::InitializeRenderLists() {
	m_BoardRender.Begin();
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_ALPHA_TEST);
		glBindTexture(GL_TEXTURE_2D, m_BoardTex);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);	glVertex2f(0.0f,			0.0f); 
			glTexCoord2f(1.0f, 0.0f);	glVertex2f((float)m_Width,	0.0f); 
			glTexCoord2f(1.0f, 1.0f);	glVertex2f((float)m_Width,	(float)m_Height); 
			glTexCoord2f(0.0f, 1.0f);	glVertex2f(0.0f, 			(float)m_Height); 
		glEnd();
		char buf[2] = {};
		c3DPoint pos(BoardOffset);
		pos.x -= 32;
		pos.y += FieldSize.y / 2.0f - bfHarrington->FontHeight() / 2.0f;
		for(char i = 'H'; i >= 'A'; --i){
			buf[0] = i;
			bfHarrington->Draw(buf, 0, pos);
			pos.y += FieldSize.y;
		}
		
		pos = BoardOffset;
		pos.y -= 32;
		pos.x += FieldSize.x / 2.0f;
		for(char i = '1'; i <= '8'; ++i){
			buf[0] =i;
			bfHarrington->Draw(buf, 0, pos);
			pos.x += FieldSize.x;
		}
		pos.y += FieldSize.y / 2.0f;
		bfHarrington->Draw("Gracz", 0, pos);
		CurrPlayerInfoPos = pos;
		CurrPlayerInfoPos.x += FieldSize.x / 2.0f;
		CurrPlayerInfoPos.y += FieldSize.y;
		glDisable(GL_ALPHA_TEST);
		glPopMatrix();
	m_BoardRender.End();
	
	int PawnSize = 25;
//------------------------pawns
	m_RedPawn.Begin();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_PawnTex);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2d(-PawnSize,	-PawnSize); 
			glTexCoord2f(1, 0); glVertex2d( PawnSize,	-PawnSize); 
			glTexCoord2f(1, 1); glVertex2d( PawnSize,	 PawnSize); 
			glTexCoord2f(0, 1); glVertex2d(-PawnSize,	 PawnSize); 
		glEnd();
	m_RedPawn.End();	
	m_WhitePawn.Begin();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_PawnTex);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2d(-PawnSize,	-PawnSize); 
			glTexCoord2f(1, 0); glVertex2d( PawnSize,	-PawnSize); 
			glTexCoord2f(1, 1); glVertex2d( PawnSize,	 PawnSize); 
			glTexCoord2f(0, 1); glVertex2d(-PawnSize,	 PawnSize); 
		glEnd();
	m_WhitePawn.End();		
//------------------------queens
	m_RedQueen.Begin();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_QueenTex);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2d(-PawnSize,	-PawnSize); 
			glTexCoord2f(1, 0); glVertex2d( PawnSize,	-PawnSize); 
			glTexCoord2f(1, 1); glVertex2d( PawnSize,	 PawnSize); 
			glTexCoord2f(0, 1); glVertex2d(-PawnSize,	 PawnSize); 
		glEnd();
	m_RedQueen.End();	
	m_WhiteQueen.Begin();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_QueenTex);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2d(-PawnSize,	-PawnSize); 
			glTexCoord2f(1, 0); glVertex2d( PawnSize,	-PawnSize); 
			glTexCoord2f(1, 1); glVertex2d( PawnSize,	 PawnSize); 
			glTexCoord2f(0, 1); glVertex2d(-PawnSize,	 PawnSize); 
		glEnd();
	m_WhiteQueen.End();		
//-------------focused fields
	cOGLRenderList* fields[] = { &m_GrayField, &m_GreenField, &m_RedField, 0, };
	c3DPoint c1(0.8f,0.8f,0.8f), c2(0.1f,1.0f,0.1f), c3(1.0f,0.1f,0.1f);
	c3DPoint* fieldColors[] = { &c1, &c2, &c3, 0};

	float fw = FieldSize.x * (2 / 3.0f);
	float fh = FieldSize.y * (2 / 3.0f);
	for(int i = 0; fields[i]; ++i){
		cOGLRenderList *lst = fields[i];
		c3DPoint &c = *fieldColors[i];
		lst->Begin();
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_FAN);
			glColor4f(c.x, c.y, c.z, 1.0f);
			glVertex2d(0, 0); 
			glColor4f(c.x, c.y, c.z, 0.0f);
			const float step = 360.0f/72.0f;
			for(int i = 0; i <= 72; ++i)
				glVertex2d(fw * sin(D2Math::DegToRad(i * step)), fh * cos(D2Math::DegToRad(i * step))); 
		glEnd();	 
		lst->End();
	}
}

void RGBToBGR(FIBITMAP *dib){
	BYTE *bits = FreeImage_GetBits(dib);
	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	for(int i = 0, j = height * width; i < j; ++i){
		BYTE b = bits[0];
		bits[0] = bits[2];
		bits[2] = b;
		bits += 3;
	}
}

void RGBAToBGRA(FIBITMAP *dib){
	BYTE *bits = FreeImage_GetBits(dib);
	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	for(int i = 0, j = height * width; i < j; ++i){
		BYTE b = bits[0];
		bits[0] = bits[2];
		bits[2] = b;
		bits += 4;
	}
}

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	cerr << "FreeImage [Format: " << FreeImage_GetFormatFromFIF(fif) << "] error: " << message << endl;
}

bool FILoadTexture(const void* Data, unsigned DataLen, GLint MipmapLevel){
	FIBITMAP *dib = 0;
	FreeImage_SetOutputMessage(FreeImageErrorHandler);

	FIMEMORY *fim = FreeImage_OpenMemory((BYTE*)Data, DataLen);
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fim);
	dib = FreeImage_LoadFromMemory(fif, fim, 0);
	FreeImage_CloseMemory(fim);
	
	if(!dib) return false;

	BYTE* bits(0);
	unsigned int width(0), height(0);
	GLuint ImgType;

	FreeImage_FlipVertical(dib);
  //	FreeImage_FlipHorizontal(dib);
	switch (FreeImage_GetBPP(dib)){
	case 32:
		RGBAToBGRA(dib);
		ImgType = GL_RGBA;
		break;
	case 24:
		RGBToBGR(dib);
		ImgType = GL_RGB;
		break;
	default:{
		FIBITMAP *dib24 = FreeImage_ConvertTo24Bits(dib);
		FreeImage_Unload(dib);
		dib = dib24;
		RGBToBGR(dib);
		ImgType = GL_RGB;
	}
	}

	bits = FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	assert((bits != 0) && (width != 0) && (height != 0));
	glTexImage2D(GL_TEXTURE_2D, MipmapLevel, ImgType, width, height, 0, ImgType, GL_UNSIGNED_BYTE, bits);
	FreeImage_Unload(dib);
	return true;
}

GLuint cApplication::LoadTexture(const char *TexFile){
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	ifstream inp(TexFile, ios::in | ios::binary);
	inp.seekg(0, ios::end);
	unsigned len = static_cast<unsigned>(inp.tellg());
//	cerr << "LT " << TexFile << "  " << len << endl;
	inp.seekg(0);
	
	char *buf = new char[len+1];
	inp.read(buf, len);
	inp.close();
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if(!FILoadTexture(buf, len, 0)){
		cerr << "Unable to load texture " << TexFile << endl;
	}
	delete buf;
	return tex;
}
