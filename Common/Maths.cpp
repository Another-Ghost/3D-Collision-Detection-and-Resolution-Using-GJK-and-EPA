/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Maths.h"
#include "../Common/Vector2.h"
#include "../Common/Vector3.h"

namespace NCL {
	namespace Maths {
		void ScreenBoxOfTri(const Vector3& v0, const Vector3& v1, const Vector3& v2, Vector2& topLeft, Vector2& bottomRight) {
			topLeft.x = std::min(v0.x, std::min(v1.x, v2.x));
			topLeft.y = std::min(v0.y, std::min(v1.y, v2.y));

			bottomRight.x = std::max(v0.x, std::max(v1.x, v2.x));
			bottomRight.y = std::max(v0.y, std::max(v1.y, v2.y));
		}

		int ScreenAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			int area =(int) (((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y)));
			return (area >> 1);
		}

		float FloatAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			float area = ((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y));
			return (area * 0.5f);
		}

		float CrossAreaOfTri(const Vector3 &a, const Vector3 &b, const Vector3 & c) {
			Vector3 area = Vector3::Cross(a - b, a - c);
			return area.Length() * 0.5f;
		}
	

		Vector3 Clamp(const Vector3& a, const Vector3&mins, const Vector3& maxs) {
			return Vector3(
				Clamp(a.x, mins.x, maxs.x),
				Clamp(a.y, mins.y, maxs.y),
				Clamp(a.z, mins.z, maxs.z)
			);
		}


		// Given segment ab and point c, computes closest point d on ab. 
		// Also returns d, d(t)=a+ t*(b - a)
		void ClosestPtPointSegment(const Vector3& c, const Vector3& a, const Vector3& b, Vector3& d)
		{
			float t;
			Vector3 ab = b - a; // Project c onto ab, but deferring divide by Dot(ab, ab) 
			t = Vector3::Dot(c - a, ab); if (t <= 0.0f) {
				// c projects outside the [a,b] interval, on the a side; clamp to a 
				t = 0.0f;
				d = a;
			}
			else {
				float denom = Vector3::Dot(ab, ab); // Always nonnegative since denom = ||ab||¡Ä2 
				if (t >= denom) {
					// c projects outside the [a,b] interval, on the b side; clamp to b 
					t = 1.0f;
					d = b;
				}
				else { // c projects inside the [a,b] interval; must do deferred divide now 
					t = t / denom;
					d = a + ab * t;
				}
			}
		}

		float ClosestPtSegmentSegment(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, Vector3& c1, Vector3& c2)
		{
			float s;
			float t;
			Vector3 d1 = q1 - p1; // Direction vector of segment S1 
			Vector3 d2 = q2 - p2; // Direction vector of segment S2 
			Vector3 r = p1 - p2;
			float a = Vector3::Dot(d1, d1); // Squared length of segment S1, always nonnegative 
			float e = Vector3::Dot(d2, d2); // Squared length of segment S2, always nonnegative 
			float f = Vector3::Dot(d2, r);
			// Check if either or both segments degenerate into points 
			if (a <= FLT_EPSILON && e <= FLT_EPSILON) {
				// Both segments degenerate into points 
				s = t = 0.0f;
				c1 = p1;
				c2 = p2;
				return Vector3::Dot(c1 - c2, c1 - c2);
			}
			if (a <= FLT_EPSILON) { // First segment degenerates into a point
				s = 0.0f;
				t = f / e; //s=0=>t= (b*s+f)/e=f/e
				t = Clamp(t, 0.0f, 1.0f);
			}
			else {
				float c = Vector3::Dot(d1, r); if (e <= FLT_EPSILON) {
					// Second segment degenerates into a point
					t = 0.0f;
					s = Clamp(-c / a, 0.0f, 1.0f); //t=0=>s= (b*t-c)/a=-c/a
				}
				else {
					// The general non-degenerate case starts here
					float b = Vector3::Dot(d1, d2);
					float denom = a * e - b * b; // Always nonnegative
					// If segments not parallel, compute closest point on L1 to L2 and 
					// clamp to segment S1. Else pick arbitrary s (here 0)
					if (denom != 0.0f) {
						s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
					}
					else s = 0.0f;
					// Compute point on L2 closest to S1(s) using
					// t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
					float tnom = b * s + f;

					// If t in [0,1] done. Else clamp t, recompute s for the new value
					// of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a 
					// and clamp s to [0, 1]
					if (tnom < 0.0f) {
						t = 0.0f;
						s = Clamp(-c / a, 0.0f, 1.0f);
					}
					else if (tnom > e) {
						t = 1.0f;
						s = Clamp((b - c) / a, 0.0f, 1.0f);
					}
					else {
						t = tnom / e;
					}
				}
			}

			c1 = p1 + d1 * s;
			c2 = p2 + d2 * t;
			return Vector3::Dot(c1 - c2, c1 - c2);
		}

		// Returns the squared distance between point c and segment ab
		float SqDistPointSegment(const Vector3& a, const Vector3& b, const Vector3& c)
		{
			Vector3 ab = b - a, ac = c - a, bc = c - b;
			float e = Vector3::Dot(ac, ab);
			// Handle cases where c projects outside ab
			if (e <= 0.0f) 
				return Vector3::Dot(ac, ac);
			float f = Vector3::Dot(ab, ab);
			if (e >= f) 
				return Vector3::Dot(bc, bc);
			// Handle cases where c projects onto ab
			return Vector3::Dot(ac, ac) - e * e / f;
		}

		float DistPointSegment(const Vector3& a, const Vector3& b, const Vector3& c)
		{
			Vector3 ab = b - a, ac = c - a, bc = c - b;
			float e = Vector3::Dot(ac, ab);
			// Handle cases where c projects outside ab
			if (e <= 0.0f)
				return sqrt(Vector3::Dot(ac, ac));
			float f = Vector3::Dot(ab, ab);
			if (e >= f)
				return sqrt(Vector3::Dot(bc, bc));
			// Handle cases where c projects onto ab
			return sqrt(Vector3::Dot(ac, ac) - e * e / f);
		}

		float LineToLineDistance(const Vector3& a1, const Vector3& a2, const Vector3& b1, const Vector3& b2)
		{
			float distance = FLT_MAX;
			Vector3 d1 = a2 - a1;
			Vector3 d2 = b2 - b1;

			Vector3 e = b1 - a1;

			Vector3 cross_e_d2 = Vector3::Cross(e, d2);
			Vector3 cross_e_d1 = Vector3::Cross(e, d1);
			Vector3 cross_d1_d2 = Vector3::Cross(d1, d2); //common perpendicular

			float t1 = Vector3::Dot(cross_e_d2, cross_d1_d2);
			float t2 = Vector3::Dot(cross_e_d1, cross_d1_d2);

			float dd = cross_d1_d2.Length();

			if (fabs(dd) < 1e-6) {
				distance = PointToLineDistance(a1, a2, b1);
				return distance;
			}

			t1 /= dd * dd;
			t2 /= dd * dd;

			Vector3 a = a1 + (a2 - a1) * t1;
			Vector3 b = b1 + (b2 - b1) * t2;
			distance = (b - a).Length();

			return distance;
		}

		float LineToLineDistance(const Vector3& a1, const Vector3& a2, const Vector3& b1, const Vector3& b2, Vector3& a, Vector3& b)
		{
			float distance = FLT_MAX;
			Vector3 d1 = a2 - a1; //dir vector
			Vector3 d2 = b2 - b1;

			Vector3 e = b1 - a1; //needed by calculation
			Vector3 cross_e_d2 = Vector3::Cross(e, d2);
			Vector3 cross_e_d1 = Vector3::Cross(e, d1);

			Vector3 cross_d1_d2 = Vector3::Cross(d1, d2); //common perpendicular

			float t1 = Vector3::Dot(cross_e_d2, cross_d1_d2);
			float t2 = Vector3::Dot(cross_e_d1, cross_d1_d2);

			float dd = cross_d1_d2.Length();

			if (fabs(dd) < 1e-6) { //Two lines are parallel
				distance = PointToLineDistance(a1, a2, b1); //Distance is equal to Distance of any point in one line to the other line in this condition
				return distance;
			}

			t1 /= dd * dd;
			t2 /= dd * dd;

			//a = a1 + a2 * t1; //Foot point of line a1a2
			//b = b1 + b2 * t2;

			a = a1 + (a2 - a1) * t1; //Foot point of line a1a2
			b = b1 + (b2 - b1) * t2; //Foot point of line b1b2
			distance = (b - a).Length();

			return distance;
		}

		float PointToLineDistance(const Vector3& p1, const Vector3& p2, const Vector3& p)
		{
			Vector3 n = (p2 - p1).Normalised();
			float k = Vector3::Dot((p - p1), n);
			Vector3 p3 = p1 + n * k;
			float distance = (p - p3).Length();
			return distance;
		}
		void Barycentric(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& p, float& u, float& v, float& w)
		{
			Vector3 v0 = b - a, v1 = c - a, v2 = p - a;
			float d00 = Vector3::Dot(v0, v0);
			float d01 = Vector3::Dot(v0, v1);
			float d11 = Vector3::Dot(v1, v1);
			float d20 = Vector3::Dot(v2, v0);
			float d21 = Vector3::Dot(v2, v1);
			float denom = d00 * d11 - d01 * d01;
			v = (d11 * d20 - d01 * d21) / denom;
			w = (d00 * d21 - d01 * d20) / denom;
			u = 1.0f - v - w;

		}
	}
}