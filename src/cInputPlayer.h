#pragma once
#include "cPlayerInterface.h"

enum {
	flag_MouseLeftDown			= 0x0001,
	flag_MouseLeftReleased		= 0x0002,
	flag_MouseLeftPressed		= 0x0004,
	flag_MouseLeft = flag_MouseLeftDown | flag_MouseLeftReleased | flag_MouseLeftPressed,
};

class cInputPlayer : public cPlayerInterface {
	cBoardCord m_GrabbedPawn, m_PreviousCord;
	const sPlayerConf &m_PlayerConf;
public:
	cInputPlayer(PlayerType SelfPlayer);

	virtual void UpdateBoard(const cMoveVector &moves, const cBoard &UpdatedBoard);

	virtual bool PushCursorPos(const cBoardCord &cord);
	virtual void SetMouseState(unsigned state);
	virtual cBoard* UseMyBoard();
};

