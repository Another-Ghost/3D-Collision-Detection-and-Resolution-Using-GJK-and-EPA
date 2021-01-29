/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector4;

		class Vector3 {
		public:
			union {
				struct {
					float x;
					float y;
					float z;
				};
				float array[3];
			};
		public:
			constexpr Vector3(void) : x(0.0f), y(0.0f), z(0.0f) {}

			constexpr Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

			Vector3(const Vector2& v2, float z = 0.0f);
			Vector3(const Vector4& v4);

			~Vector3(void) {}

			Vector3 Normalised() const {
				Vector3 temp(x, y, z);
				temp.Normalise();
				return temp;
			}

			void			Normalise() {
				float length = Length();

				if (length != 0.0f) {
					length = 1.0f / length;
					x = x * length;
					y = y * length;
					z = z * length;
				}
			}

			float	Length() const {
				return sqrt((x*x) + (y*y) + (z*z));
			}

			constexpr float	LengthSquared() const {
				return ((x*x) + (y*y) + (z*z));
			}

			constexpr float		GetMaxElement() const {
				float v = x;
				if (y > v) {
					v = y;
				}
				if (z > v) {
					v = z;
				}
				return v;
			}

			float		GetAbsMaxElement() const {
				float v = abs(x);
				if (abs(y) > v) {
					v = abs(y);
				}
				if (abs(z) > v) {
					v = abs(z);
				}
				return v;
			}

			static constexpr float	Dot(const Vector3 &a, const Vector3 &b) {
				return (a.x*b.x) + (a.y*b.y) + (a.z*b.z);
			}

			static Vector3	Cross(const Vector3 &a, const Vector3 &b) {
				return Vector3((a.y*b.z) - (a.z*b.y), (a.z*b.x) - (a.x*b.z), (a.x*b.y) - (a.y*b.x));
			}

			inline Vector3  operator+(const Vector3  &a) const {
				return Vector3(x + a.x, y + a.y, z + a.z);
			}

			inline Vector3  operator-(const Vector3  &a) const {
				return Vector3(x - a.x, y - a.y, z - a.z);
			}

			inline Vector3  operator-() const {
				return Vector3(-x, -y, -z);
			}

			inline Vector3  operator*(float a)	const {
				return Vector3(x * a, y * a, z * a);
			}

			inline Vector3  operator*(const Vector3  &a) const {
				return Vector3(x * a.x, y * a.y, z * a.z);
			}

			inline Vector3  operator/(const Vector3  &a) const {
				return Vector3(x / a.x, y / a.y, z / a.z);
			};

			inline Vector3  operator/(float v) const {
				return Vector3(x / v, y / v, z / v);
			};

			inline constexpr void operator+=(const Vector3  &a) {
				x += a.x;
				y += a.y;
				z += a.z;
			}

			inline void operator-=(const Vector3  &a) {
				x -= a.x;
				y -= a.y;
				z -= a.z;
			}


			inline void operator*=(const Vector3  &a) {
				x *= a.x;
				y *= a.y;
				z *= a.z;
			}

			inline void operator/=(const Vector3  &a) {
				x /= a.x;
				y /= a.y;
				z /= a.z;
			}

			inline void operator*=(float f) {
				x *= f;
				y *= f;
				z *= f;
			}

			inline void operator/=(float f) {
				x /= f;
				y /= f;
				z /= f;
			}

			inline float operator[](int i) const {
				return array[i];
			}

			inline float& operator[](int i) {
				return array[i];
			}

			inline bool	operator==(const Vector3 &A)const { return (A.x == x && A.y == y && A.z == z) ? true : false; };
			inline bool	operator!=(const Vector3 &A)const { return (A.x == x && A.y == y && A.z == z) ? false : true; };

			inline friend std::ostream& operator<<(std::ostream& o, const Vector3& v) {
				o << "Vector3(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
				return o;
			}
		};
	}
}
