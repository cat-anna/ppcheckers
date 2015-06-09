#ifndef OpenGLMiscH
#define OpenGLMiscH

class cOGLRenderList {
	GLuint m_ListHandle;
    cOGLRenderList(const cOGLRenderList& lst): m_ListHandle(0) {}//no copy constructor;
public:
	cOGLRenderList(): m_ListHandle(0) { }
	~cOGLRenderList(){ Clear(); };
	void Clear(){
		glDeleteLists(m_ListHandle, 1);
		m_ListHandle = 0;
	}
	void Begin(){
		if(m_ListHandle) Clear();
		m_ListHandle = glGenLists(1);  
		glNewList(m_ListHandle, GL_COMPILE);  
	}
	void End(){	glEndList(); }
    bool IsReady() const { return m_ListHandle != 0; }
    void Call() const { glCallList(m_ListHandle); }
	
	void Call(const c3DPoint &D){
		glPushMatrix();
		glTranslatef(D.x, D.y, D.z);
		Call();
		glPopMatrix();
	}
};

#endif
