#ifndef _D2_MATH_
//#if !defined(_COLLISION_MATH_)
//|| defined(_COLLISION_MATH_NOT_INLINE)
#define _D2_MATH_

#include <cmath>
#include <stdexcept>
#include <string>
#include "c3DPoint.h"

#define pi 3.14159f
#define pi_half (pi / 2.0f)

namespace D2Math {
	
template <class T>
inline T sqr(const T &f){
	return f * f;
}
/*
template <typename T>
inline T &min (T &a, T &b){
	if(a < b)return a;
	return b;
}

template <typename T>
inline T &max (T &a, T &b){
	if(a < b)return b;
	return a;
}*/

template <typename T>
inline T sgn (T a){
	if(a < 0) return static_cast<T>(-1);
	if(a > 0) return static_cast<T>(1);
	return static_cast<T>(0);
}

class eNoSolution : public std::runtime_error {
public:
	eNoSolution(const std::string &msg) : std::runtime_error(msg){};
};

class cm2DLine{ 
public:
	float A, B, C;
	cm2DLine(float a = 0, float b = 0, float c = 0):A(a), B(b), C(c){};
	cm2DLine(const c3DPoint &P1, const c3DPoint &P2){
		if(P1.z == P2.z){
			A = 0;
			B = 1.0f;
			C = -P2.z;
		} else {
			A = 1.0f;
			B = (P1.x - P2.x);
			if(B != 0) B /= (P1.z - P2.z);
			C = -P1.x - P1.z * B;
		}
	}
/*
Ax + By + C = 0;
By = -Ax - C
y = -A/B - C/B [B!=0]
*/
	float AngleWithOX(){
		if(A == 0) return 0;
		if(B == 0) return pi_half;
		return atan(A/B);
	}

	float Distance(const c3DPoint &point){
		return fabs(A * point.x + B * point.z + C) / sqrt(sqr(A) + sqr(B));
	}

	bool PointOnLine(const c3DPoint &point, float AllowedError = 0.00001f){
		return fabs(A * point.x + B * point.z + C) < AllowedError;
	}

	cm2DLine &NormalThoroughPoint(const c3DPoint &point, cm2DLine &Output){
		Output.A = B;
		Output.B = A;
		Output.C = -Output.A * point.x - Output.B * point.z;
		return Output;
	}

	bool CommonPoint(const cm2DLine &line, c3DPoint &output){
		float W = A * line.B - line.A * B;
		if(W == 0) return false;
		float Wx = line.C * B - line.B * C;
		float Wy = line.A * C - A * line.C;
		output.x = Wx / W;
		output.z = Wy / W;
		return true;
	} 
};

inline float DegToRad(float deg){
	return deg * pi / 180.0f;
}

inline c3DPoint &PointOnCircle(const c3DPoint &Center, float Angle, float Radius, c3DPoint &output){
	output.x = Center.x + Radius * sin(Angle);
	output.z = Center.z - Radius * cos(Angle);
	return output;
}

inline c3DPoint &PointOnCircleXZ(const c3DPoint &Center, float Angle, float Radius, c3DPoint &output){
	output.x = Center.x + Radius * sin(Angle);
	output.z = Center.z - Radius * cos(Angle);
	return output;
}
/*
inline bool PointInSection2D(const c3DPoint &a, const c3DPoint &b, const c3DPoint &Point){
	return (min(a.x, b.x) <= Point.x) && (Point.x <= max(a.x, b.x)) && (min(a.z, b.z) <= Point.z) && (Point.z <= max(a.z, b.z));
}*/
/*
inline bool SectionAndCircleCollision(const c3DPoint &P1, const c3DPoint &P2, const c3DPoint &Center, const float &Radius){
	if(Center.DistanceXZ(P1) <= Radius) return true;
	if(Center.DistanceXZ(P2) <= Radius) return true;	
	cm2DLine Section(P1, P2), Normal;
	Section.NormalThoroughPoint(Center, Normal);
	c3DPoint Common;
	if(!Normal.CommonPoint(Section, Common)) return false;
	if(!PointInSection2D(P1, P2, Common)) return false;
	if(Section.Distance(Center) <= Radius) return true;
	return false;
}

inline bool PointsAreEqual(const c3DPoint &P1, const c3DPoint &P2, float Error = 0.01){
	return fabs(P1.x - P2.x) <= Error && fabs(P1.y - P2.y) <= Error && fabs(P1.z - P2.z) <= Error;
}*/

};//namespace D2Math

#undef pi
#undef pi_half

#endif
