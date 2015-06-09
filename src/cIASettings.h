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

	//operator sumowania(��czenia wag) wag
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
	Ta klasa przechowuje zestaw wag oraz ustawie�
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
// szukaj bi� �a�cuchowych
	bool fLookForChainKills;

//inne
	int iMaximumRecursionDeph;

//wagi (s�|b�d�) cz�sto mno�one przez siebie, wiec raczej nie stosowa� warto�ci ujemnych
//zalecam logarytmiczny rozk�ad tzn 0.1 = 10 (s� to liczby przeciwne jak -1 oraz 1)

//przesuni�cie si� do przodu
	cMoveValue wMove;
//przesuni�cie si� do przodu nie dotyczy pionk�w jesli fAllowBackwardMoves=true
	cMoveValue wBackMove;

//pierwsze bicie
	cMoveValue wKill;
//je�li bity pionek to kr�lowa waga za bicie(jedna z powy�szych) mo�ona jest przez t� warto��
	cMoveValue wQueenKill;
//tylko dla kr�l�wek, gdy bij� wi�cej ni� 1 pionek na raz, aplikowane tyle razy ile by�o bi�
	cMoveValue wMultipleKill;
//zamiana pionka w kr�low�
	cMoveValue wMakeQueen;
	
//pomi� ruchy je�li maj� wag� mniejsz� ni� ...
	cMoveValue wSkipMovesWithValueLesserThan;

//Kosz zej�cia pozim ni�ej (nale�y pomno�y�)
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

//pionki mog� porusza� si� do przodu, dotyczy tylko pionki
	bool fAllowBackwardMoves; 
//mo�na bi� do ty�u nie dotyczy kr�l�wek
	bool fAllowBackwardKills; 
// mo�na bi� do ty�u tylko je�li jest to co najmniej drugie bicie, tylk gdy fAllowBackwardKills = false,
	bool fAllowBackwardChainKills;//nie dotyczy kr�l�wek
};

//Globalne podstawowe ustawienia
extern cIASettings AISettings;
extern cGameSettings GameSettings;