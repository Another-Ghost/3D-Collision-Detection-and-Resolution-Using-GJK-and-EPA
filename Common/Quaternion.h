/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <iostream>

namespace NCL {
	namespace Maths {
		class Matrix3;
		class Matrix4;
		class Vector3;

		class Quaternion {
		public:
			union {
				struct {
					float x;
					float y;
					float z;
					float w;
				};
				float array[4];
			};
		public:
			Quaternion(void);
			Quaternion(float x, float y, float z, float w);
			Quaternion(const Vector3& vector, float w);

			Quaternion(const Matrix3 &m);
			Quaternion(const Matrix4 &m);

			~Quaternion(void);

			void	Normalise();
			
			static float Dot(const Quaternion &a, const Quaternion &b);

			static Quaternion	Lerp(const Quaternion &from, const Quaternion &to, float by);
			static Quaternion	Slerp(const Quaternion &from, const Quaternion &to, float by);

			Vector3		ToEuler() const;
			Quaternion	Conjugate() const;
			void		CalculateW();	//builds 4th component when loading in shortened, 3 component quaternions

			static Quaternion EulerAnglesToQuaternion(float pitch, float yaw, float roll);
			static Quaternion AxisAngleToQuaterion(const Vector3& vector, float degrees);

			inline bool  operator ==(const Quaternion &from)	const {
				if (x != from.x || y != from.y || z != from.z || w != from.w) {
					return false;
				}
				return true;
			}

			inline bool  operator !=(const Quaternion &from)	const {
				if (x != from.x || y != from.y || z != from.z || w != from.w) {
					return true;
				}
				return false;
			}

			inline Quaternion  operator *(const Quaternion &b)	const {
				return Quaternion(
					(x * b.w) + (w * b.x) + (y * b.z) - (z * b.y),
					(y * b.w) + (w * b.y) + (z * b.x) - (x * b.z),
					(z * b.w) + (w * b.z) + (x * b.y) - (y * b.x),
					(w * b.w) - (x * b.x) - (y * b.y) - (z * b.z)
				);
			}

			Vector3		operator *(const Vector3 &a)	const;

			inline Quaternion  operator *(const float &a)		const {
				return Quaternion(x*a, y*a, z*a, w*a);
			}

			inline Quaternion  operator *=(const float &a) {
				*this = *this * a;
				return *this;
			}

			inline Quaternion  operator -()	const {
				return Quaternion(-x, -y, -z, -w);
			}

			inline Quaternion  operator -(const Quaternion &a)	const {
				return Quaternion(x - a.x, y - a.y, z - a.z, w - a.w);
			}

			inline Quaternion  operator -=(const Quaternion &a) {
				*this = *this - a;
				return *this;
			}

			inline Quaternion  operator +(const Quaternion &a)	const {
				return Quaternion(x + a.x, y + a.y, z + a.z, w + a.w);
			}

			inline Quaternion  operator +=(const Quaternion &a) {
				*this = *this + a;
				return *this;
			}

			inline float operator [](const int i) const {
				return array[i];
			}

			inline friend std::ostream& operator <<(std::ostream& o, const Quaternion& q);
			inline friend std::istream& operator >>(std::istream& i, Quaternion &v);
		};

		std::ostream& operator<<(std::ostream& o, const Quaternion& q) {
			o	<< "Quaternion("
				<< q.x; o << ","
				<< q.y; o << ","
				<< q.z; o << ","
				<< q.w;
			return o;
		}

		std::istream& operator >> (std::istream& i, Quaternion &v) {
			char ignore;
			i >> std::skipws >> v.x >> ignore >> v.y >> ignore >> v.z >> ignore >> v.w >> std::noskipws;
			return i;
		}
	}
}