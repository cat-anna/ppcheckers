
#include "cInputPlayer.h"
using namespace std;

enum {
	flag_Pawn_Grabbed			= 0x0010,
	
	flag_multikill				= 0x10000,
};

cInputPlayer::cInputPlayer(PlayerType SelfPlayer): cPlayerInterface(SelfPlayer, 0), m_PlayerConf(PlayerConf[SelfPlayer]), m_GrabbedPawn(-1), m_PreviousCord(-1) {

}

cBoard* cInputPlayer::UseMyBoard() {
	return &m_Board;
}

void cInputPlayer::UpdateBoard(const cMoveVector &moves, const cBoard &UpdatedBoard) {
	m_Board = UpdatedBoard;
	m_PreviousCord = -1;
	m_GrabbedPawn = -1;
	m_Flags &= ~flag_Move_is_ready;
	m_LastMove.MovesFromParent.clear();
}

void cInputPlayer::SetMouseState(unsigned state) { 
	m_Flags &= ~flag_MouseLeft;
	m_Flags |= state;
}

bool cInputPlayer::PushCursorPos(const cBoardCord &cord) {
	bool moveend = false;
	bool outOfBoard = !cord;

	if(!cord) {
		if(m_Flags & flag_Pawn_Grabbed && !(m_Flags & flag_multikill)){
		//	if(!)
			m_GrabbedPawn = -1;
			m_PreviousCord = -1;
		//	m_LastMove.MovesFromParent.clear();
			m_Flags &= ~(flag_Pawn_Grabbed | flag_MouseLeft);
		}
	} else
	if(m_Flags & flag_Pawn_Grabbed) {
		m_PreviousCord = cord;
		bool can = true;	//pawn can be moved to new field
		bool willkill = false; //pawn will kill
		//int ox, oy - grabbed pawn;
		cBoardCord delta = cord - m_GrabbedPawn;
		cBoardCord ABSdelta = delta.abs();
		cBoardCord diag = delta.signum();
		cField &Fcord = m_Board.at(cord);
		cField &Fgrabbed = m_Board.at(m_GrabbedPawn);
		m_LastMove.MovesFromParent.back() = cord;
		if(!Fcord.empty()) 
			can = false; //pole docelowe nie jest puste
		else
		if(ABSdelta.x() != ABSdelta.y())  
			can = false;  //nie jest na przek¹tnej
		else
		if(Fgrabbed.IsPawn()){
			//chkkill - czy zewnêtrzne warunki pozwalaj¹ na bicie
			bool chkkill = GameSettings.fAllowBackwardKills || m_PlayerConf.BackwardDir != diag.y();
			if(!chkkill && GameSettings.fAllowBackwardChainKills && (m_Flags & flag_multikill)) chkkill = true;

			if(chkkill && ABSdelta.x() == 2 && ABSdelta.y() == 2 && m_Board.at(cord - diag).IsOppositePawn(m_Player)){
				//zwyk³y pionek bije po przek¹tnej
			} else
			//pionek nie porusza siê do ty³u
			if(m_PlayerConf.BackwardDir == diag.y() && !GameSettings.fAllowBackwardMoves){
				can = false;
			} else
			//check whether pawn is trying to do too long move
				if(ABSdelta.x() != 1 || ABSdelta.y() != 1)
					can = false; //zwyk³y pionek próbuje zbyt d³ugi ruch po przek¹tnej
		} 
		if(can){
			for(cBoardCord pt = m_GrabbedPawn + diag; pt != cord && pt; pt += diag){
				cField &fp = m_Board.at(pt);
				if(fp.empty()) continue;//filed is empty
				if(fp.IsPlayerPawn(m_Player) ){
					can = false;
					fp.SetStatus(cField::fsRedSelect);
				} else {
					fp.SetStatus(cField::fsGreenSelect);
					willkill = true;
				}
			}
		} else {
		}
		if(!willkill && (m_Flags & flag_multikill)){
		//	m_Flags &= ~flag_multikill;
			can = false;
		}
		if(can) m_Board.at(cord).SetStatus(cField::fsGreenSelect);
		else m_Board.at(cord).SetStatus(cField::fsRedSelect);

		if(m_Flags & flag_MouseLeftReleased){
			bool waskill = false;
			if(!can && (m_Flags & flag_multikill)){
                m_Flags &= ~flag_multikill;
                moveend = true;
			} else
			if(can){				
				for(cBoardCord pt = m_GrabbedPawn + diag; pt != cord; pt += diag){//clear fields
					cout << pt << endl;
					cField &fp = m_Board.at(pt);
					if(fp.empty()) continue;//filed is empty
					if(fp.IsPlayerPawn(m_Player))continue; //if is player pawn skip
					fp = 0;
					waskill = true;
				}
				//przesuñ pionek oraz weŸ jego typ
				cField &cur = m_Board.at(cord) = m_Board.at(m_GrabbedPawn);
				m_Board.at(m_GrabbedPawn) = 0;
				if(!waskill){
                    moveend = true;
					m_Flags &= ~flag_multikill;
					m_LastMove.MovesFromParent.pop_back();
				} else {
					m_Flags &= ~(flag_MouseLeftReleased);
					m_Flags |= flag_multikill;
					m_GrabbedPawn = cord;
					m_LastMove.MovesFromParent.PushMove(cord);
					cur.SetStatus(cField::fsPickedUp);
				}

				//czy pionek dotar³ do lini królówki
				if(cur.IsPawn() && cord.y() == m_PlayerConf.QueenLine) {
					cur.MakeMeQueen();
				}
			}
			if(!waskill){
				m_Flags &= ~(flag_Pawn_Grabbed | flag_MouseLeftReleased);
				cField &cur = m_Board.at(m_GrabbedPawn);
				cur.SetStatus(cField::fsNone);
				m_GrabbedPawn = -1;
				m_PreviousCord = -1;
			}
            if(moveend){
				m_LastMove.Board = m_Board;
				//cout << "BBB " << m_LastMove.MovesFromParent.size() << endl;
				if(!waskill)
					m_LastMove.MovesFromParent.pop_back();
				//m_LastMove.MovesFromParent.PushMove(cord);
				m_Flags |= flag_Move_is_ready;
            }
		} 
    } else {
		cField &f = m_Board.at(cord);
		if(f.empty()){
		} else {
			//za pomoc¹ myszki mo¿na obecnie graæ tylko bia³ym graczem
			if(f.IsPlayerPawn(m_Player) && !m_GrabbedPawn){
				f.SetStatus(cField::fsGreenSelect);
				if(m_Flags & flag_MouseLeftPressed){
					m_Flags &= ~(flag_MouseLeftPressed | flag_MouseLeftReleased);
					m_Flags |= flag_Pawn_Grabbed;
					m_LastMove.MovesFromParent.clear();
					m_LastMove.MovesFromParent.PushMove(cord);
					m_LastMove.MovesFromParent.PushMove(cord);
					m_GrabbedPawn = cord;
				}
			} else {
				f.SetStatus(cField::fsRedSelect);
			}
		}
	}
	return false;
}
	