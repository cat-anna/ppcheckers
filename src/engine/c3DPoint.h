#ifndef C3DPOINT_H
#define C3DPOINT_H

#include <math.h>

#ifndef c3DPoint_TEMPLATE

typedef float PType;

#else

template <class PType = float>

#endif
class c3DPoint {
public:
    ~c3DPoint() {};
    c3DPoint() {
        x = y = z = 0;
    }
    c3DPoint(const c3DPoint& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    c3DPoint(const PType ax, const PType ay, const PType az) {
        x = ax;
        y = ay;
        z = az;
    }
    c3DPoint(const PType v) {
        x = y = z = v;
    }
	
	operator float*() { return &x; }

    c3DPoint &operator=(const c3DPoint& other) {
        if (this == &other) return *this; // handle self assignment
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

	c3DPoint &operator=(const PType V) {
        x = y = z = V;
        return *this;
    }

    c3DPoint &operator+=(const c3DPoint& other) {
        x += other.x;
        y += other.y;
		z += other.z;
        return *this;
    }
    c3DPoint &operator-=(const c3DPoint& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    c3DPoint &operator*=(const PType V) {
        x *= V;
        y *= V;
        z *= V;
        return *this;
	}
	template <class DIV>
	c3DPoint &operator/=(const DIV V) { x /= V; y /= V; z /= V; return *this; }

    c3DPoint operator-(const c3DPoint& other)const {
        return c3DPoint(x - other.x, y - other.y, z -other.z);
	}

	c3DPoint operator+(const c3DPoint& other)const {
        return c3DPoint(x + other.x, y + other.y, z + other.z);
	}

	c3DPoint operator*(const PType V)const {
		return c3DPoint(x * V, y * V, z * V);
	}
	template <class DIV>
    c3DPoint operator/(const DIV V)const {
		return c3DPoint(x / V, y / V, z / V);
    }

    bool operator ==(const c3DPoint& other) const{
		return x == other.x && y == other.y && z == other.z;
	}
	bool operator ==(const PType V) const{
        return x == V && y == V && z == V;
    }
    bool operator !=(const PType V) const{
        return x != V && y != V && z != V;
    }

    PType Distance(const c3DPoint &v)const {
        return pow(pow(x - v.x, 2.0f) +
                   pow(y - v.y, 2.0f) +
                   pow(z - v.z, 2.0f), 0.5f);
    }

    PType DistanceXZ(const c3DPoint &v)const {
        return pow(pow(x - v.x, 2.0f) +
                   pow(z - v.z, 2.0f), 0.5f);
    }

    PType Length()const {
        return pow(pow(x, 2.0f) + pow(y, 2.0f) + pow(z, 2.0f), 0.5f);
    }

    PType LengthXZ()const {
        return pow(pow(x, 2.0f) + pow(z, 2.0f), 0.5f);
    }

    c3DPoint Versor() const {
    	return c3DPoint(*this) /= Length();
    }

    c3DPoint VersorXZ() const {
    	return c3DPoint(*this) /= LengthXZ();
    }

    PType x;
    PType y;
    PType z;

#ifdef HEADER_PUGIXML_HPP
	const c3DPoint& SaveToXML(pugi::xml_node Node, const char **Captions = 0)const{
		const char *c3DPoint_XYZ_Captions[] = { "X", "Y", "Z", };
		if(!Captions) Captions = c3DPoint_XYZ_Captions;
		Node.append_attribute(Captions[0]) = x;
		Node.append_attribute(Captions[1]) = y;
		Node.append_attribute(Captions[2]) = z;
		return *this;
	};

	c3DPoint& LoadFromXML(const pugi::xml_node Node, const char **Captions = 0){
		if(!Node)return (*this = 0);
		const char *c3DPoint_XYZ_Captions[] = { "X", "Y", "Z", };
		if(!Captions) Captions = c3DPoint_XYZ_Captions;
		x = Node.attribute(Captions[0]).as_float();
		y = Node.attribute(Captions[1]).as_float();
		z = Node.attribute(Captions[2]).as_float();
		return *this;
	}
#endif
};

#endif // C3DPOINT_H
