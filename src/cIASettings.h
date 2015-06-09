#pragma once

#include <cmath>

class cMoveValue {
	float m_value;
	bool m_NegativeMode;
public:
	cMoveValue(float val = 0): m_value(val), m_NegativeMode(false) { }
	cMoveValue(const cMoveValue& val): m_value(val.m_value), m_NegativeMode(val.m_NegativeMode) { }

	operator float () const { return m_value; }

	cMoveValue& operator = (const cMoveValue& value) {
		m_value = value.m_value;
		m_NegativeMode = value.m_NegativeMode;
		return *this;
	}

	cMoveValue& operator = (float value) {
		m_value = value;
		return *this;
	}

	//operator sumowania(³¹czenia wag) wag
	cMoveValue& operator &= (const cMoveValue& val) {
		float DynVal = val.m_value;
		if(m_NegativeMode) DynVal *= -1;
	//	m_value *= DynVal;
		m_value += DynVal;
		return *this;
	}

	cMoveValue& operator += (const cMoveValue& val) {
		float DynVal = val.m_value;
	//	if(m_NegativeMode) DynVal *= -1;
	//	m_value *= DynVal;
		m_value += DynVal;
		return *this;
	}

	cMoveValue absolute() const { return abs(m_value); }

	cMoveValue operator & (float DynVal) {
		if(m_NegativeMode) DynVal *= -1;
	//	return cMoveValue(m_value * DynVal);
		return cMoveValue(m_value + DynVal);
	}

	bool operator > (const cMoveValue& val) const {
		return m_value > val.m_value;
	}

	bool operator < (const cMoveValue& val) const {
		return m_value < val.m_value;
	}

	void SetNegativeMode(bool enabled) {
		m_NegativeMode = enabled;
	}
};

/*
	Ta klasa przechowuje zestaw wag oraz ustawieñ
	jest to klasa - kontener dlatego mam w nosie settery i gettery oraz polimorfizm
*/
class cIASettings {
public:
	cIASettings() {
		//ustawienia standardowe
		fLookForChainKills = true;

		wMove = 1.0f;
		wBackMove = -1.0f;

		wMakeQueen = 10;

		wKill = 100;
		wQueenKill = 200;
		wMultipleKill = 10;

		wSkipMovesWithValueLesserThan = 0;

		wLowerLevelCost = 0.2f;//unused

		iMaximumRecursionDeph = 5;
	}

//flagi
// szukaj biæ ³añcuchowych
	bool fLookForChainKills;

//inne
	int iMaximumRecursionDeph;

//wagi (s¹|bêd¹) czêsto mno¿one przez siebie, wiec raczej nie stosowaæ wartoœci ujemnych
//zalecam logarytmiczny rozk³ad tzn 0.1 = 10 (s¹ to liczby przeciwne jak -1 oraz 1)

//przesuniêcie siê do przodu
	cMoveValue wMove;
//przesuniêcie siê do przodu nie dotyczy pionków jesli fAllowBackwardMoves=true
	cMoveValue wBackMove;

//pierwsze bicie
	cMoveValue wKill;
//jeœli bity pionek to królowa waga za bicie(jedna z powy¿szych) mo¿ona jest przez t¹ wartoœæ
	cMoveValue wQueenKill;
//tylko dla królówek, gdy bij¹ wiêcej ni¿ 1 pionek na raz, aplikowane tyle razy ile by³o biæ
	cMoveValue wMultipleKill;
//zamiana pionka w królow¹
	cMoveValue wMakeQueen;
	
//pomiñ ruchy jeœli maj¹ wagê mniejsz¹ ni¿ ...
	cMoveValue wSkipMovesWithValueLesserThan;

//Kosz zejœcia pozim ni¿ej (nale¿y pomno¿yæ)
	cMoveValue wLowerLevelCost;
};

//globalne ustawienia gry
class cGameSettings {
public:
	cGameSettings() {
		fAllowBackwardMoves = false;
		fAllowBackwardKills = false;
		fAllowBackwardChainKills = true;
		
		WaitTimeBeforeAIPlayer = 1;
	};

	float WaitTimeBeforeAIPlayer;//w sekundach

//pionki mog¹ poruszaæ siê do przodu, dotyczy tylko pionki
	bool fAllowBackwardMoves; 
//mo¿na biæ do ty³u nie dotyczy królówek
	bool fAllowBackwardKills; 
// mo¿na biæ do ty³u tylko jeœli jest to co najmniej drugie bicie, tylk gdy fAllowBackwardKills = false,
	bool fAllowBackwardChainKills;//nie dotyczy królówek
};

//Globalne podstawowe ustawienia
extern cIASettings AISettings;
extern cGameSettings GameSettings;