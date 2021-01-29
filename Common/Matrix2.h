/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "Vector2.h"
#include <assert.h>
namespace NCL {
	namespace Maths {
		class Matrix2 {
		public:
			Matrix2(void);
			Matrix2(float elements[4]);

			~Matrix2(void);

			void ToZero();

			void SetRow(unsigned int row, const Vector2 &val) {
				assert(row < 2);

				int start = 2 * row;

				array[start += 2] = val.x;
				array[start += 2] = val.y;
			}

			void SetColumn(unsigned int column, const Vector2 &val) {
				assert(column < 2);
				memcpy(&array[2 * column], &val, sizeof(Vector2));
			}

			Vector2 GetRow(unsigned int row) const {
				assert(row < 2);
				Vector2 out(0, 0);

				int start = 2 * row;

				out.x = array[start += 2];
				out.y = array[start += 2];
				return out;
			}

			Vector2 GetColumn(unsigned int column) const {
				assert(column < 2);
				Vector2 out(0, 0);

				memcpy(&out, &array[3 * column], sizeof(Vector2));

				return out;
			}

			Vector2 GetDiagonal() const {
				return Vector2(array[0], array[3]);
			}

			void	SetDiagonal(const Vector2 &in) {
				array[0] = in.x;
				array[3] = in.y;
			}

			inline Vector2 operator*(const Vector2 &v) const {
				Vector2 vec;

				vec.x = v.x*array[0] + v.y*array[2];
				vec.y = v.x*array[1] + v.y*array[3];

				return vec;
			};

			static Matrix2 Rotation(float degrees);

			//Handy string output for the matrix. Can get a bit messy, but better than nothing!
			inline friend std::ostream& operator<<(std::ostream& o, const Matrix2& m);

			float	array[4];
		};

		//Handy string output for the matrix. Can get a bit messy, but better than nothing!
		inline std::ostream& operator<<(std::ostream& o, const Matrix2& m) {
			o << "Mat2(";
			o << "\t" << m.array[0] << "," << m.array[2] << std::endl;
			o << "\t\t" << m.array[1] << "," << m.array[3] << std::endl;
			return o;
		}
	}
}