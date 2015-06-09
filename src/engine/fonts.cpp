#include <App.h>
#include <fonts.h>
#include <fstream>
#include <cstring>

cBitmapFont::cBitmapFont(const char *bfd, const char* Tex){
	ifstream inp(bfd, ios::in | ios::binary);
	inp.seekg(0, std::ios::end);
	unsigned len = inp.tellg();
	inp.seekg(0, std::ios::beg);
	if(!inp) throw "!Font inp";
	inp.read((char*)&m_BFD, sizeof(cBFDHeader));
	//if(inp.c != sizeof(cBFDHeader)) ;//throw "!bfd";
	inp.close();
	m_Texture = Application->LoadTexture(Tex);
}

cBitmapFont::~cBitmapFont(){
//TODO: kill texture
}

void cBitmapFont::Draw(const char *Text, float height, const c3DPoint &Pos) const{
	if(!Text) return;

	float x = Pos.x, y = Pos.y/*, z = Pos.z*/;
	float h = static_cast<float>(m_BFD.CharWidth), w;
	if(height > 0) h = height;
	w = h;
	float w_mult = w / static_cast<float>(m_BFD.CharWidth);
	
	unsigned fx = m_BFD.Width  / m_BFD.CharWidth;
	unsigned fy = m_BFD.Height / m_BFD.CharHeight; 
	float Cx = m_BFD.Width  / static_cast<float>(m_BFD.CharWidth);
	float Cy = m_BFD.Height / static_cast<float>(m_BFD.CharHeight);
	float dw = 1 / Cx;
	float dh = 1 / Cy;
	unsigned delta = m_BFD.BeginingKey;

	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glDisable(GL_BLEND);
	glBegin(GL_QUADS);	
	while(*Text){
		unsigned kid = static_cast<unsigned>(*Text) - delta;
		if(kid > 255) kid = fx;
		unsigned kol  = kid % fx;
		unsigned line = kid / fx;
		float u = kol  / Cx;
		float v = line / Cy;
		 
		glTexCoord2f(u + dw, v     );		glVertex2f(x + w, y    ); 
		glTexCoord2f(u     , v     );		glVertex2f(x    , y    ); 
		glTexCoord2f(u     , v + dh);		glVertex2f(x    , y + h); 
		glTexCoord2f(u + dw, v + dh);		glVertex2f(x + w, y + h); 

		x += (m_BFD.KeyWidths[kid] + 1) * w_mult;
		++Text;
	}
	glEnd();
	glEnable(GL_BLEND);
}

void cBitmapFont::DrawEx(const char *Text, float height, cColorCaller* ExFun, const c3DPoint &Pos) const{
	if(!Text || ! ExFun) return;

	float x = Pos.x, y = Pos.y/*, z = Pos.z*/;
	float h = static_cast<float>(m_BFD.CharWidth), w;
	if(height > 0) h = height;
	w = h;
	float w_mult = w / static_cast<float>(m_BFD.CharWidth);
	
	unsigned fx = m_BFD.Width  / m_BFD.CharWidth;
	unsigned fy = m_BFD.Height / m_BFD.CharHeight; 
	float Cx = m_BFD.Width  / static_cast<float>(m_BFD.CharWidth);
	float Cy = m_BFD.Height / static_cast<float>(m_BFD.CharHeight);
	float dw = 1 / Cx;
	float dh = 1 / Cy;
	unsigned delta = m_BFD.BeginingKey;
//	typedef void(*ExFun_p)(void *Param,  unsigned Type)
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glDisable(GL_BLEND);
	glBegin(GL_QUADS);	
	ExFun->call(deBegin);
	while(*Text){
		unsigned kid = static_cast<unsigned>(*Text) - delta;
		if(kid > 255) kid = fx;
		unsigned kol  = kid % fx;
		unsigned line = kid / fx;
		float u = kol  / Cx;
		float v = line / Cy;
		ExFun->call(deNextChar);
		
		ExFun->call(deRT); glTexCoord2f(u + dw, v     );		glVertex2f(x + w, y    ); 
		ExFun->call(deLT); glTexCoord2f(u     , v     );		glVertex2f(x    , y    ); 
		ExFun->call(deLB); glTexCoord2f(u     , v + dh);		glVertex2f(x    , y + h); 
		ExFun->call(deRB); glTexCoord2f(u + dw, v + dh);		glVertex2f(x + w, y + h); 

		x += (m_BFD.KeyWidths[kid] + 1) * w_mult;
		++Text;
	}
	ExFun->call(deEnd);
	glEnd();
	glEnable(GL_BLEND);
}

cBitmapFont::cBFDHeader::cBFDHeader(){
	Width = Height = CharWidth = CharHeight = 0;
	BeginingKey = 0;
	memset(KeyWidths, 0, 256);
}
