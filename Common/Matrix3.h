/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <assert.h>
#include <algorithm>
#include <iostream>

namespace NCL {
	namespace Maths {
		class Matrix2;
		class Matrix4;
		class Vector3;
		class Quaternion;

		class Matrix3
		{
		public:
			Matrix3(void);
			Matrix3(float elements[9]);
			Matrix3(const Matrix2 &m4);
			Matrix3(const Matrix4 &m4);
			Matrix3(const Quaternion& quat);

			~Matrix3(void);

			//Set all matrix values to zero
			void	ToZero();

			Vector3 GetRow(unsigned int row) const;
			void	SetRow(unsigned int row, const Vector3 &val);

			Vector3 GetColumn(unsigned int column) const;
			void	SetColumn(unsigned int column, const Vector3 &val);

			Vector3 GetDiagonal() const;
			void	SetDiagonal(const Vector3 &in);

			Vector3 ToEuler() const;

			inline Matrix3 Absolute() const {
				Matrix3 m;

				for (int i = 0; i < 9; ++i) {
					m.array[i] = std::abs(array[i]);
				}

				return m;
			}

			inline Matrix3 Transposed() const {
				Matrix3 temp = *this;
				temp.Transpose();
				return temp;
			}

			inline void Transpose() {
				float tempValues[3];

				tempValues[0] = array[3];
				tempValues[1] = array[6];
				tempValues[2] = array[7];

				array[3] = array[1];
				array[6] = array[2];
				array[7] = array[5];

				array[1] = tempValues[0];
				array[2] = tempValues[1];
				array[5] = tempValues[2];
			}

			Vector3 operator*(const Vector3 &v) const;

			inline Matrix3 operator*(const Matrix3 &a) const {
				Matrix3 out;
				//Students! You should be able to think up a really easy way of speeding this up...
				for (unsigned int r = 0; r < 3; ++r) {
					for (unsigned int c = 0; c < 3; ++c) {
						out.array[c + (r * 3)] = 0.0f;
						for (unsigned int i = 0; i < 3; ++i) {
							out.array[c + (r * 3)] += this->array[c + (i * 3)] * a.array[(r * 3) + i];
						}
					}
				}
				return out;
			}


			inline Matrix3 operator*(const float a) const {
				Matrix3 out;
				//Students! You should be able to think up a really easy way of speeding this up...
				for (unsigned int r = 0; r < 3; ++r) {
					for (unsigned int c = 0; c < 3; ++c) {
						out.array[c + (r * 3)] *= a;
					}
				}
				return out;
			}
			//Creates a rotation matrix that rotates by 'degrees' around the 'axis'
			//Analogous to glRotatef
			static Matrix3 Rotation(float degrees, const Vector3 &axis);

			//Creates a scaling matrix (puts the 'scale' vector down the diagonal)
			//Analogous to glScalef
			static Matrix3 Scale(const Vector3 &scale);

			static Matrix3 FromEuler(const Vector3 &euler);
		public:
			float array[9];
		};

		//Handy string output for the matrix. Can get a bit messy, but better than nothing!
		inline std::ostream& operator<<(std::ostream& o, const Matrix3& m) {
			o << m.array[0] << "," << m.array[1] << "," << m.array[2] << std::endl;
			o << m.array[3] << "," << m.array[4] << "," << m.array[5] << std::endl;
			o << m.array[6] << "," << m.array[7] << "," << m.array[8];
			return o;
		}

		inline std::istream& operator >> (std::istream& i, Matrix3& m) {
			char ignore;
			i >> std::skipws;
			i >> m.array[0] >> ignore >> m.array[1] >> ignore >> m.array[2];
			i >> m.array[3] >> ignore >> m.array[4] >> ignore >> m.array[5];
			i >> m.array[6] >> ignore >> m.array[7] >> ignore >> m.array[8];

			return i;
		}
	}
}