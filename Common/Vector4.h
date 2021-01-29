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
		class Vector3;
		class Vector2;

		class Vector4 {

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
			constexpr Vector4(void) : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

			constexpr Vector4(float xVal, float yVal, float zVal, float wVal) : x(xVal), y(yVal), z(zVal), w(wVal) {}

			Vector4(const Vector3& v3, float w = 0.0f);
			Vector4(const Vector2& v2, float z = 0.0f, float w = 0.0f);

			~Vector4(void) {}

			Vector4 Normalised() const {
				Vector4 temp(x, y, z, w);
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
					w = w * length;
				}
			}

			float	Length() const {
				return sqrt((x*x) + (y*y) + (z*z) + (w * w));
			}

			constexpr float	LengthSquared() const {
				return ((x*x) + (y*y) + (z*z) + (w * w));
			}

			constexpr float		GetMaxElement() const {
				float v = x;
				if (y > v) {
					v = y;
				}
				if (z > v) {
					v = z;
				}
				if (w > v) {
					v = w;
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
				if (abs(w) > v) {
					v = abs(w);
				}
				return v;
			}

			static float	Dot(const Vector4 &a, const Vector4 &b) {
				return (a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w);
			}

			inline Vector4  operator+(const Vector4  &a) const {
				return Vector4(x + a.x, y + a.y, z + a.z, w + a.w);
			}

			inline Vector4  operator-(const Vector4  &a) const {
				return Vector4(x - a.x, y - a.y, z - a.z, w - a.w);
			}

			inline Vector4  operator-() const {
				return Vector4(-x, -y, -z, -w);
			}

			inline Vector4  operator*(float a)	const {
				return Vector4(x * a, y * a, z * a, w * a);
			}

			inline Vector4  operator*(const Vector4  &a) const {
				return Vector4(x * a.x, y * a.y, z * a.z, w * a.w);
			}

			inline Vector4  operator/(const Vector4  &a) const {
				return Vector4(x / a.x, y / a.y, z / a.z, w / a.w);
			};

			inline Vector4  operator/(float v) const {
				return Vector4(x / v, y / v, z / v, w / v);
			};

			inline constexpr void operator+=(const Vector4  &a) {
				x += a.x;
				y += a.y;
				z += a.z;
				w += a.w;
			}

			inline void operator-=(const Vector4  &a) {
				x -= a.x;
				y -= a.y;
				z -= a.z;
				w -= a.w;
			}


			inline void operator*=(const Vector4  &a) {
				x *= a.x;
				y *= a.y;
				z *= a.z;
				w *= a.w;
			}

			inline void operator/=(const Vector4  &a) {
				x /= a.x;
				y /= a.y;
				z /= a.z;
				w /= a.w;
			}

			inline void operator*=(float f) {
				x *= f;
				y *= f;
				z *= f;
				w *= f;
			}

			inline void operator/=(float f) {
				x /= f;
				y /= f;
				z /= f;
				w /= f;
			}

			inline float operator[](int i) const {
				return array[i];
			}

			inline float& operator[](int i) {
				return array[i];
			}

			inline bool	operator==(const Vector4 &A)const { return (A.x == x && A.y == y && A.z == z && A.w == w) ? true : false; };
			inline bool	operator!=(const Vector4 &A)const { return (A.x == x && A.y == y && A.z == z && A.w == w) ? false : true; };

			inline friend std::ostream& operator<<(std::ostream& o, const Vector4& v) {
				o << "Vector4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")" << std::endl;
				return o;
			}
		};

		static Vector4 COLOUR_BLACK(0, 0, 0, 1.0f);
		static Vector4 COLOUR_WHITE(1, 1, 1, 1.0f);
		static Vector4 HOMOGENEOUS(0, 0, 0, 1.0f);
	}
}