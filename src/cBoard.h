#pragma once

#include <c3DPoint.h>
#include "cBoardCoord.h"

enum PieceType {
	ptNone			= 0,

	ptWhitePawn		= 1,
	ptWhiteQueen	= 2,
	ptBlackPawn		= 3,
	ptBlackQueen	= 4,
};

enum PlayerType {
	Player_Error,
	PlayerWhite,
	PlayerBlack,

	PlayerMaxValue,
};

struct sPlayerConf {
	const char* Id;
	int BaseLine;
	int QueenLine;
	int BackwardDir;
	PieceType Queen;
	PieceType Pawn;
};

extern const sPlayerConf PlayerConf[PlayerMaxValue];

extern const c3DPoint FieldSize, BoardOffset;

inline PlayerType OppositePlayer(PlayerType Player){
	if(Player == PlayerWhite) return PlayerBlack;
	return PlayerWhite;
}


/*
inline bool IsPlayerPawn(unsigned Player, unsigned Pawn){
    if(!Pawn || !Player) return false;
	return (Player == PlayerWhite && (Pawn == ptWhitePawn || Pawn == ptWhiteQueen)) ||
		   (Player == PlayerBlack && (Pawn == ptBlackPawn || Pawn == ptBlackQueen));
}

inline bool IsPawn(unsigned Pawn){
	return Pawn == ptWhitePawn || Pawn == ptBlackPawn;
}

inline bool IsQueen(unsigned Queen){
	return Queen == ptWhiteQueen || Queen == ptBlackQueen;
}

inline unsigned MakeMeQueen(unsigned Pawn){
    if(Pawn == ptWhitePawn) return ptWhiteQueen;
    if(Pawn == ptBlackPawn) return ptBlackQueen;
    return 0;
}*/

//definicja kierunku przekątnej
struct sDiagonal {
	cBoardCord delta;//przesunięcie wspórzędnych
	PlayerType isBackwardFor;//dla kogo ta przekątna jest tą "do tyłu"
};
//definicja 4 przekątnych
extern const int DiagonalCount;
extern const sDiagonal DiagonalDef[];

class cField {
    unsigned short m_value;
	unsigned short m_FieldStatus;
public:
	cField():m_FieldStatus(0), m_value(0) {

	}
    //czy należy do gracza
    bool IsPlayerPawn(unsigned Player) const {
        if(!m_value || !Player) return false;
        return (Player == PlayerWhite && (m_value == ptWhitePawn || m_value == ptWhiteQueen)) ||
               (Player == PlayerBlack && (m_value == ptBlackPawn || m_value == ptBlackQueen));
    }
	
	bool IsOppositePawn(PlayerType Player) const {
        if(!m_value || !Player) return false;
        return IsPlayerPawn(OppositePlayer(Player));
    }

    //czy jest pionkiem
    bool IsPawn() const {
        return m_value == ptWhitePawn || m_value == ptBlackPawn;
    }
    //czy jest królową
    bool IsQueen() const {
        return m_value == ptWhiteQueen || m_value == ptBlackQueen;
    }
    //przemnień w królową
    void MakeMeQueen() {
        if(m_value == ptWhitePawn) m_value = ptWhiteQueen;
        if(m_value == ptBlackPawn) m_value = ptBlackQueen;
    }

    bool empty() const { return !m_value; }
    operator unsigned short () const { return m_value; }
	operator unsigned () const { return m_value; }
    operator unsigned short& () { return m_value; }
    unsigned operator =(unsigned value) { return m_value = value; }

	enum eFieldStatus {
		fsNone,
		fsPickedUp,//podniesiony, nie renderuj
		fsRedSelect,
		fsGreenSelect,
		fsGraySelect,
		fsPossibleMove,//pionka tak naprawdę tam nie ma, ale trzeba renderować
	};

	eFieldStatus GetStatus() const { return (eFieldStatus)m_FieldStatus; };
	void SetStatus(eFieldStatus status) { m_FieldStatus = status; }
};

class cBoard {
public:
	enum {
        BoardWidth = cBoardCord::BoardWidth,
        BoardHeight = cBoardCord::BoardHeight,
	};

    cField &at(int x, int y) {
		return m_Board[y * BoardWidth + x];
	}
    cField &at(const cBoardCord& p1) {
		return m_Board[p1.Linear()];
	}

    cField &at(int linear) {
		return m_Board[linear];
	}

	void swap(const cBoardCord& p1, const cBoardCord& p2){
		//To się da napisać lepiej ale mi się nie chce
		cField tmp = at(p1);
		at(p1) = at(p2);
		at(p2) = tmp;
	}

	void swap(int x1, int y1, int x2, int y2){
		//To się da napisać lepiej ale mi się nie chce
		cField tmp = at(x1, y1);
		at(x1, y1) = at(x2, y2);
		at(x2, y2) = tmp;
	}

	cBoard(void);
	~cBoard(void);

	void SetStartPosition();
	void DumpToConsole();
private:
    cField m_Board[BoardWidth * BoardHeight];
};
