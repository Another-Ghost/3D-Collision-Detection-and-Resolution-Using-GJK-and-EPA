/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <algorithm>

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector3;

		//It's pi(ish)...
		static const float		PI = 3.14159265358979323846f;

		//It's pi...divided by 360.0f!
		static const float		PI_OVER_360 = PI / 360.0f;

		//Radians to degrees
		inline float RadiansToDegrees(float rads) {
			return rads * 180.0f / PI;
		};

		//Degrees to radians
		inline float DegreesToRadians(float degs) {
			return degs * PI / 180.0f;
		};

		template<class T>
		inline T Clamp(T value, T min, T max) {
			if (value < min) {
				return min;
			}
			if (value > max) {
				return max;
			}
			return value;
		}

		Vector3 Clamp(const Vector3& a, const Vector3&mins, const Vector3& maxs);

		template<class T>
		inline T Lerp(const T& a, const T&b, float by) {
			return (a * (1.0f - by) + b*by);
		}

		void ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight);

		int ScreenAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);
		float FloatAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);

		float CrossAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c);

		// Given segment ab and point c, computes closest point d on ab. 
		// Also returns d, d(t)=a+ t*(b - a)
		void ClosestPtPointSegment(const Vector3& c, const Vector3& a, const Vector3& b, Vector3& d);

		// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and 
		// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared 
		// distance between between S1(s) and S2(t)
		float ClosestPtSegmentSegment(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& c1, Vector3& c2);

		// Returns the squared distance between point c and segment ab
		float SqDistPointSegment(const Vector3& a, const Vector3& b, const Vector3& c);

		float DistPointSegment(const Vector3& a, const Vector3& b, const Vector3& c);

		float LineToLineDistance(const Vector3& a1, const Vector3& a2, const Vector3& b1, const Vector3& b2);
		float LineToLineDistance(const Vector3& a1, const Vector3& a2, const Vector3& b1, const Vector3& b2, Vector3& a, Vector3& b);
		float PointToLineDistance(const Vector3& p1, const Vector3& p2, const Vector3& p);

		void Barycentric(const Vector3 & a, const Vector3 & b, const Vector3 & c, const Vector3 & p, float& u, float& v, float& w);

	}
}