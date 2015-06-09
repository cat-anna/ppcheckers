#pragma once

#include <iostream>
#include <D2Math.h>

class cBoardCord {
	int m_x, m_y;
public:
	enum {
        BoardWidth = 8,
        BoardHeight = 8,
	};

	cBoardCord(int x, int y) : m_x(x), m_y(y) {}
	cBoardCord(const cBoardCord& other) : m_x(other.m_x), m_y(other.m_y) {}
	cBoardCord(int linear) : m_x(linear % BoardWidth), m_y(linear / BoardWidth) {}

	unsigned Linear() const {
		return m_x + m_y * BoardWidth;
	}

	operator unsigned () const {
		return Linear();
	}

	int& x() { return m_x; }
	int& y() { return m_y; }

	int x() const { return m_x; }
	int y() const { return m_y; }

	operator bool () const {
		if(m_x < 0 || m_y < 0) return false;
		if(m_x >= BoardWidth || m_y >= BoardHeight) return false;
		return true;
	}

	cBoardCord abs() const {
		return cBoardCord(::abs(m_x),::abs(m_y));
	}

	cBoardCord signum() const {
		return cBoardCord(D2Math::sgn(m_x),D2Math::sgn(m_y));
	}

	cBoardCord operator * (unsigned val) const {
		return cBoardCord(m_x * val, m_y * val);
	}
	cBoardCord operator + (const cBoardCord& other) const {
		return cBoardCord(m_x + other.m_x, m_y + other.m_y);
	}

	cBoardCord operator - (const cBoardCord& other) const {
		return cBoardCord(m_x - other.m_x, m_y - other.m_y);
	}

	cBoardCord& operator = (const cBoardCord& other) {
		if(this == &other) return *this;
		m_x = other.m_x;
		m_y = other.m_y;
		return *this;
	}

	cBoardCord& operator = (int v) {
		m_x = v;
		m_y = v;
		return *this;
	}

	cBoardCord& operator *= (unsigned val) {
		m_x *= val;
		m_y *= val;
		return *this;
	}

	cBoardCord& operator += (const cBoardCord& other) {
		m_x += other.m_x;
		m_y += other.m_y;
		return *this;
	}

	cBoardCord& operator -= (const cBoardCord& other) {
		m_x -= other.m_x;
		m_y -= other.m_y;
		return *this;
	}

	bool operator < (const cBoardCord& other) {
		return (m_x < other.m_x) && (m_y < other.m_y);
	}

	bool operator <= (const cBoardCord& other) {
		return (m_x <= other.m_x) && (m_y <= other.m_y);
	}

	bool operator == (const cBoardCord& other) {
		return (m_x == other.m_x) && (m_y == other.m_y);
	}

	bool operator != (const cBoardCord& other) {
		return (m_x != other.m_x) || (m_y != other.m_y);
	}

	friend std::ostream& operator << (std::ostream& o, const cBoardCord& c) {
		o << "\tx:" << c.m_x << "\ty:" << c.m_y;
		return o;
	}
};
