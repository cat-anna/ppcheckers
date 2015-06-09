#include "cBoard.h"
#include <cstring>
#include <iostream>
using namespace std;

const c3DPoint FieldSize(64.5,64.5,0), BoardOffset(142,42,0);

const int DiagonalCount = 4;
const sDiagonal DiagonalDef[] = {
	{ cBoardCord(-1, -1), PlayerBlack, },
	{ cBoardCord( 1, -1), PlayerBlack, },

	{ cBoardCord(-1,  1), PlayerWhite, },
	{ cBoardCord( 1,  1), PlayerWhite, },
};

//tablica informacji o playerach
const sPlayerConf PlayerConf[PlayerMaxValue] = {
	{ "x",  0, 0, 0, ptNone, ptNone, }, //value for Player_Error
//dzia³amy na odwróconym Y
//BaseLine, QueenLine, ...
	{ "Wht", 7, 0,  1, ptWhiteQueen, ptWhitePawn, }, //value for PlayerWhite
	{ "Red", 0, 7, -1, ptBlackQueen, ptBlackPawn, }, //value for PlayerBlack
};

cBoard::cBoard(void) {
	SetStartPosition();
}


cBoard::~cBoard(void) {
}

unsigned StartPos[64] = {

	ptBlackPawn,0,ptBlackPawn,0,ptBlackPawn,0,ptBlackPawn,0,//h
	0,ptBlackPawn,0,ptBlackPawn,0,ptBlackPawn,0,ptBlackPawn,//g
	ptBlackPawn,0,ptBlackPawn,0,ptBlackPawn,0,ptBlackPawn,0,//f
    0,0,0,0,0,0,0,0,//e
    0,0,0,0,0,0,0,0,//d
	0,ptWhitePawn,0,ptWhitePawn,0,ptWhitePawn,0,ptWhitePawn,//c
	ptWhitePawn,0,ptWhitePawn,0,ptWhitePawn,0,ptWhitePawn,0,//b
	0,ptWhitePawn,0,ptWhitePawn,0,ptWhitePawn,0,ptWhitePawn,//a

};

void cBoard::SetStartPosition() {
	memcpy(m_Board, StartPos, BoardHeight * BoardWidth * sizeof(unsigned));
}

void cBoard::DumpToConsole() {
	cout << endl;
	for(int i = 0; i < BoardHeight; ++i){
		for(int j = 0; j < BoardWidth; ++j) 
			if(((i+j) % 2 == 0))
				cout << (unsigned)at(j, i);
			else
				cout << " ";
		cout << endl;
	}
}


