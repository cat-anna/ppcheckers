#ifndef id83A8DEBE_CA84_487B_AD63A63AECADC9F9
#define id83A8DEBE_CA84_487B_AD63A63AECADC9F9

#include <OpenGLMisc.h>
#include <env.h>

class cBitmapFont {
	struct cBFDHeader {
		cBFDHeader();
		unsigned Width, Height, CharWidth, CharHeight;
		unsigned char BeginingKey, KeyWidths[256];
	};

	GLuint m_Texture;
	cBFDHeader m_BFD;
public:
	cBitmapFont(const char *bfd, const char* Tex);
	~cBitmapFont();
	enum {
		deBegin,
		deNextChar,
		deRT, deLT, deLB, deRB,
		deEnd,
	};
	struct cColorCaller {
		virtual void call(unsigned mode) = 0;
	};
	int FontHeight() const { return m_BFD.CharHeight;}
	void DrawEx(const char *Text, float height, cColorCaller* ExFun, const c3DPoint &Pos = c3DPoint()) const;
	void Draw(const char *Text, float height, const c3DPoint &Pos = c3DPoint()) const;
	void GenText(cOGLRenderList & list, const char *Text, float height, const c3DPoint &Pos = c3DPoint()){
		list.Begin();
		Draw(Text, height, Pos);
		list.End();
	}

};

#endif // header
