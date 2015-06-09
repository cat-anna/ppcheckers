#include "cBrain.h"
#include <cstring>
#include <iostream>
using namespace std;

//struktura definiuj¹ca jedn¹ przek¹tn¹ wzglêdn¹ dla pionka, ka¿dy ma takie 4

cBrain::cBrain(PlayerType SelfPlayer) : cPlayerInterface(SelfPlayer, flag_Is_AI_player) {
	m_IAS = &AISettings;
}

cBrain::~cBrain(void) {
}

void cBrain::UpdateBoard(const cMoveVector &moves, const cBoard &UpdatedBoard) {
    m_Flags |= flag_Alg_Is_Working;
	m_Flags &= ~flag_Move_is_ready;
	m_Board = UpdatedBoard;
}

enum {
	//zablokuj zwyk³e przesniêcie
	flag_disable_regular_move			= 0x0001,

	flag_is_looking_for_chain_kill		= 0x0010,

	flag_lower_level					= 0x0100,
	flag_opposite_player				= 0x0200,
	
	//gotowe zestawy flag
	flagset_FindNextKill		= flag_disable_regular_move | flag_is_looking_for_chain_kill,
};


//sprawdŸ czy pionek zostaje królow¹
//Parametry:
//Pawn - wspó³rzêdne pionka
//Player - do którego playera gracz nale¿y
//CurrentMove - obecnie rozpatrywany ruch
bool cBrain::CheckCoronation(const cBoardCord &Pawn, PlayerType Player, sMoveInfo &CurrentMove) {
//pobie¿ dane o graczu
	const sPlayerConf & pc = PlayerConf[Player];
//sprawdŸ czy staje siê królówk¹
	if(Pawn.y() != pc.QueenLine) return true;
//dokonaj operacji zmiany p³ci
	CurrentMove.Board.at(Pawn) = pc.Queen;
	CurrentMove.ThisMoveValue &= m_IAS->wMakeQueen;
	CurrentMove.debugID.push_back(9);
	return false;
}

//Nale¿y j¹ napisaæ porz¹dnie.
//Parametry:
//Pawnx, Pawny - pocz¹tkowe wspó³rzêdne pionka
//isPawn - jest pionkiem czy królówk¹
//flags - patrz enum powy¿ej
//Player - którego playera rozwa¿amy
//BeginFrom - warunki pocz¹tkowe obliczeñ (m.in. plansza lub poprzednie przeskoki podczas bicia)
//output - tutaj dodawane s¹ mo¿liwe ruchy
void cBrain::CheckPawn(int Pawnx, int Pawny, bool isPawn, unsigned flags, PlayerType Player, sMoveInfo &BeginFrom, sMoveInfo &Parent, MoveList *LevelOut){
	bool lowerLvl = flags & flag_lower_level;
	bool opposite = flags & flag_opposite_player;
	flags &= ~flag_lower_level;
	
	for(int did = 0; did < DiagonalCount; ++did) {//did - id diagonali
		const sDiagonal &diag = DiagonalDef[did];
		//if(isPawn && diag.isBackwardFor == Player && !m_IAS->fAllowBackwardMoves)
		//	continue; //jeœli ruch do ty³u nie jest dozwolony i jest pionkiem, pomiñ przek¹tn¹	

		cBoardCord coord(Pawnx, Pawny);
		bool isBackward = diag.isBackwardFor == Player;
		int KillCount = 0;//iloœæ zbitych bierek w jednym, ruchu;
		sMoveInfo *NewMove = BeginFrom.Duplicate();

		NewMove->MovesFromParent.PushMove(Pawnx, Pawny);
		NewMove->ThisMoveValue.SetNegativeMode(opposite);
		NewMove->debugID.push_back(-1);

		for(unsigned i = 1; i <= cBoard::BoardHeight; ++i) {//przeleæ po wrzystkich mo¿liwych polach na diagonali
			if(isPawn){//ograniczenia rucu zwyk³ego pionka
				if(i > 2) break;//niewa¿ne jak w jednym ruchu nie mo¿ne wiêcej pól
				if(KillCount != 1 && i > 1) break;//jeœli nie by³o bicia,nie mo¿na sprawdzaæ drugiego pola
			}

			//policz wspó³rzêdne itego pola na przek¹tnej;
			cBoardCord diagCord = coord + diag.delta * i;
			if(!diagCord) break; //jeœli poza plansz¹ opuœæ pêtlê
			//policz wspó³rzêdne nastêpnego pola

			cField &Field = BeginFrom.Board.at(diagCord);
			bool FieldEmpty = Field.empty();

			//jeœli próbujemy coœ zrobiæ do ty³u
			if(isBackward){
				//tego if'a mo¿na zapisaæ lepiej, ale za to mniej czytelnie
				if(flags & flag_is_looking_for_chain_kill) {
					//jeœli szukamy bicia ³añcuchowego do ty³u
					if(isPawn && !GameSettings.fAllowBackwardChainKills) //ale nie jest dozwolone
						break; //opuœæ pêtlê
				} else {
					if(FieldEmpty){
						//próbujemy siê przesun¹æ do ty³u
						if(isPawn && !GameSettings.fAllowBackwardMoves) //ale nie jest dozwolone
							break; //opuœæ pêtlê
					} else {
						//bêdziemy testowaæ bicie do ty³u
						if(isPawn && !GameSettings.fAllowBackwardKills) //ale nie jest dozwolone
							break; //opuœæ pêtlê
					}
				}
			}

			if(FieldEmpty) {
				if(KillCount == 0 && (flags & flag_is_looking_for_chain_kill))//jeœli nie by³o bicia a szukamu bicia ³aænuchowego 
					continue;//kontunuuj sprawdzanie przek¹tnej
				//mo¿emy tam pionka przesun¹æ
				sMoveInfo *m = NewMove->Duplicate();
				if(isPawn)//jeœli jest królówk¹ nie ma to sensu oraz generuje dziwne zachowanie
					m->ThisMoveValue &= (isBackward)?(m_IAS->wBackMove):(m_IAS->wMove);
				m->MovesFromParent.PushMove(diagCord);
				m->Board.swap(diagCord, coord);
				m->debugID.push_back(i*100 + diagCord.x() * 10 + diagCord.y());
				m->debugID.push_back(1);
				if(isPawn)//sprawdŸ czy pionek staje sie królow¹
					isPawn = CheckCoronation(diagCord, Player, *m);
				m->Parent = &Parent;
				bool added = false;
			//	cout << 
				if(m->ThisMoveValue.absolute() >= m_IAS->wSkipMovesWithValueLesserThan) {
					m->TreeMoveValue += m->ThisMoveValue; 
					Parent.AvailableMoves.push_back(m);
					if(LevelOut)
						LevelOut->push_back(m);
					added = true;
				}
				if(KillCount > 0 && m_IAS->fLookForChainKills)
					CheckPawn(diagCord.x(), diagCord.y(), isPawn, flags | flagset_FindNextKill, Player, *m, Parent, LevelOut);
				if(!added)
					delete m;
			} else {
				if(Field.IsPlayerPawn(Player))
					break; //pionek w polu nale¿y do badanego gracza, nie przeszukujemy dalej przek¹tnej
				//na polu jest przeciwnik
				NewMove->Board.at(diagCord) = 0;//usuñ przeciwnika
				//zmieñ wagê w zale¿noœci od tego co bijemy
				NewMove->ThisMoveValue &= (Field.IsQueen())?(m_IAS->wQueenKill):(m_IAS->wKill);
				if(KillCount > 0)//jeœli jest to drugie kub kolejne bicie w jednej lini
					NewMove->ThisMoveValue &= m_IAS->wMultipleKill;//dodaj dodatkow¹ wagê
				++KillCount;//zwiêksz licznik bicia po lini
				NewMove->debugID.push_back(4);
			}
		}
		delete NewMove;
	}
}

bool cBrain::ProcessLevel(int Level, MoveList &PrvLvlList, MoveList &LastLvlList, PlayerType Player){
	cout << "Scanning level " << Level << ".  " << flush;
	MoveList LvlList;
	unsigned flags = 0;
	if(Player != m_Player) flags |= flag_opposite_player;
	if(Level > 1) flags |= flag_lower_level;

	for(auto it = PrvLvlList.begin(), jt = PrvLvlList.end(); it != jt; ++it){
		for(int i = 0; i < cBoard::BoardHeight; ++i)
		for(int j = 0; j < cBoard::BoardWidth; ++j){
			cField &f = m_Board.at(i, j);
			if(f.empty() || !f.IsPlayerPawn(Player)) continue;
			CheckPawn(i, j, f.IsPawn(), flags, Player, *(*it), *(*it), &LvlList);
		}
	}
	cout << "Found: " << LvlList.size() << " moves." << endl;
	if(LvlList.empty()){
		cout << "level " << Level << " is empty!" << endl;
		return false;
	}

	if(Level >= m_IAS->iMaximumRecursionDeph){
		cout << "Scanning is done" << endl;
		LastLvlList = LvlList;
		return true;
	}
	if(ProcessLevel(Level + 1, LvlList, LastLvlList, OppositePlayer(Player))) return true;
	LastLvlList = LvlList;
	cout << "Level " << Level << " returned" << endl;
	return true;
}

void cBrain::Work() {
	m_KnownMoves.clear();
	m_KnownMoves.Board = m_Board;
	m_KnownMoves.Parent = 0;
	MoveList rootLvl, LastLvl;
	rootLvl.push_back(&m_KnownMoves);
	ProcessLevel(1, rootLvl, LastLvl, m_Player);

	if(LastLvl.empty()){
		//nie znaleziono mo¿liwych ruchów
		cout << "cBrain no more moves!" << endl;
		return;
	} else { 
		//posortuj mo¿liwe ruchy
		cout << "Sorting moves" << endl;
		LastLvl.sort(sMoveInfoSortByValue);

		sMoveInfo *lst = LastLvl.front(); //przepisz "najlepszy" ruch
		MoveList path;
		while(lst && lst->Parent != &m_KnownMoves) {
			path.push_back(lst);
			lst = lst->Parent; //przejdŸ w górê drzewa szukaj¹c pierwszego ruchu
		}
		path.push_back(lst);
		m_LastMove.CopyRoot(*lst);

		cout << "--------------------------------------------------------------------------------------\n";
		int step = 0;
		while(!path.empty()){
			lst = path.back();
			path.pop_back();
			printf("dumping step %d.   points: %f\n", step, (float)lst->ThisMoveValue);
			++step;
			cout << "step dbg: ";
			for(std::vector<int>::iterator it = lst->debugID.begin(); it != lst->debugID.end(); ++it)
			cout << " " << *it;
			cout << endl;
			lst->Board.DumpToConsole();
			cout << endl;
			for(cMoveVector::iterator it = lst->MovesFromParent.begin(); it != lst->MovesFromParent.end(); ++it){
				cBoardCord c(*it);
				cout << c << endl;
			}		
			cout << endl;
			cout << endl;
		}	
	}
//	m_KnownMoves.clear();
    m_Flags |= flag_Move_is_ready;
	m_Flags &= ~flag_Alg_Is_Working;
}
