#pragma once
#include "cPlayerInterface.h"
#include <vector>
#include <list>

class cBrain : public cPlayerInterface{
public:
    cBrain(PlayerType SelfPlayer);
	~cBrain(void);

	void UpdateBoard(const cMoveVector &moves, const cBoard &UpdatedBoard);
	void Work();
protected:
	void CheckPawn(int Pawnx, int Pawny, bool isPawn, unsigned flags, PlayerType Player, sMoveInfo &BeginFrom, sMoveInfo &Parent, MoveList *LevelOut);
	bool CheckCoronation(const cBoardCord &Pawn, PlayerType Player, sMoveInfo &CurrentMove);
	bool ProcessLevel(int Level, MoveList &PrvLvlList, MoveList &LastLvlList, PlayerType Player);
private:
    sMoveInfo m_KnownMoves;
	cIASettings *m_IAS;
};
