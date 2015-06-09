#pragma once

#include <vector>
#include <list>
#include "cIASettings.h"
#include "cBoard.h"
#include <stack>

class cMoveVector : public std::vector<unsigned> {
public:
	void PushMove(int x, int y){
		push_back(y * cBoard::BoardWidth + x);
	}
	void PushMove(unsigned val){
		push_back(val);
	}
};

enum {
	flag_Alg_Is_Working			= 0x0001,
	flag_Thread_is_running		= 0x0002,
	flag_Is_AI_player			= 0x0004,
	flag_Move_is_ready			= 0x0008,
};

struct sMoveInfo;
typedef std::list<sMoveInfo*> MoveList;

struct sMoveInfo {
private:
	typedef std::stack<void*> cInstancesStack;

	static cInstancesStack Stack;
public:
	void* operator new (unsigned s) {
		if(!Stack.empty()){
			void *p = Stack.top();
			Stack.pop();
			return p;
		}
		return malloc(s);
	}

	void operator delete (void* ptr){
		Stack.push(ptr);
	}

	sMoveInfo(){
		Parent = 0;
		ThisMoveValue = 0;//1;
		TreeMoveValue = 0;
	}

	~sMoveInfo(){
		clear();
	}

	sMoveInfo *Parent;
	cBoard Board;//szachownica PO tym ruchu
	//ruchy wykonane w tym kroku
	//pierwszy zawsze jest punkt pocz¹tkowy
	//ostatni jest punkt koñcowy
	//pomiêdzy mog¹ byæ dodatowe wspó³rzêdne jeœli by³o bicie wielokrotne
	cMoveVector MovesFromParent;	
	cMoveValue ThisMoveValue;//waga tego ruchu
	cMoveValue TreeMoveValue;//waga tego oraz poprzednich
	MoveList AvailableMoves;//Wszystkie ruchy jakie (mog¹|maj¹ sens) byæ wykonane póŸniej
	
	void CopyRoot(sMoveInfo &src) { //kopiuje tylko bierz¹cy poziom
		clear();
		MovesFromParent = src.MovesFromParent;
		AvailableMoves.clear();
		ThisMoveValue = src.ThisMoveValue;
		Board = src.Board;
	}

	void clear() {
		MovesFromParent.clear();
		for(auto it = AvailableMoves.begin(), jt = AvailableMoves.end(); it != jt; ++it)
			delete *it;
		AvailableMoves.clear();
		ThisMoveValue = 0;//1;
		TreeMoveValue = 0;
	}

	sMoveInfo* Duplicate() const{
		sMoveInfo *r = new sMoveInfo();
		r->Board = Board;
		r->MovesFromParent = MovesFromParent;
		r->Parent = Parent;
		r->ThisMoveValue = ThisMoveValue;
		r->TreeMoveValue = 0;//wartoœæ nie jest duplikowana
		r->debugID = debugID;
		r->debugID.push_back(3);
		return r;
	}

	std::vector<int> debugID;//œledzenie powstawania i modyfikacji rekordu ruchu
};

inline bool sMoveInfoSortByValue(const sMoveInfo *m1, const sMoveInfo *m2){
	return m1->TreeMoveValue > m2->TreeMoveValue;
};

class cPlayerInterface {
protected:	
	PlayerType m_Player;	//którym graczem gra ta 
	sMoveInfo m_LastMove;	//ostatnio wykonany ruch
	cBoard m_Board;			//obecna szachownica
	unsigned m_Flags;
public:
	cPlayerInterface(PlayerType Player, unsigned initFlags = 0): m_Player(Player), m_Flags(initFlags) {
	}

	PlayerType GetPlayerType() const { return m_Player; }
	const sMoveInfo &PeekLastMove() const { return m_LastMove; }
	bool IsMoveReady() const { return (m_Flags & flag_Move_is_ready) != 0; }
	bool IsAIPlayer() const { return (m_Flags & flag_Is_AI_player) != 0; }
	 
	virtual void Work() {};
	virtual void UpdateBoard(const cMoveVector &moves, const cBoard &UpdatedBoard) = 0;
	virtual void Initalize() { }

	virtual bool PushCursorPos(const cBoardCord &cord) { return true; }//
	virtual void SetMouseState(unsigned state) { };
	
	virtual cBoard* UseMyBoard() { return 0;}
	
};

