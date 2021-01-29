/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Matrix3.h"
#include "Matrix2.h"
#include "Matrix4.h"

#include "Maths.h"
#include "Vector3.h"
#include "Quaternion.h"

using namespace NCL;
using namespace NCL::Maths;

Matrix3::Matrix3(void)	{
	for (int i = 0; i < 9; ++i) {
		array[i] = 0.0f;
	}
	array[0] = 1.0f;
	array[4] = 1.0f;
	array[8] = 1.0f;
}

Matrix3::Matrix3(float elements[9]) {
	array[0] = elements[0];
	array[1] = elements[1];
	array[2] = elements[2];

	array[3] = elements[4];
	array[4] = elements[5];
	array[5] = elements[6];

	array[6] = elements[8];
	array[7] = elements[9];
	array[8] = elements[10];
}

Matrix3::Matrix3(const Matrix4 &m4) {
	array[0] = m4.array[0];
	array[1] = m4.array[1];
	array[2] = m4.array[2];

	array[3] = m4.array[4];
	array[4] = m4.array[5];
	array[5] = m4.array[6];

	array[6] = m4.array[8];
	array[7] = m4.array[9];
	array[8] = m4.array[10];
}

Matrix3::Matrix3(const Matrix2 &m2) {
	array[0] = m2.array[0];
	array[1] = m2.array[1];
	array[2] = 0;

	array[3] = m2.array[2];
	array[4] = m2.array[3];
	array[5] = 0.0f;

	array[6] = 0.0f;
	array[7] = 0.0f;
	array[8] = 1.0f;
}

Matrix3::Matrix3(const Quaternion &quat) {
	float yy = quat.y * quat.y;
	float zz = quat.z * quat.z;
	float xy = quat.x * quat.y;
	float zw = quat.z * quat.w;
	float xz = quat.x * quat.z;
	float yw = quat.y * quat.w;
	float xx = quat.x * quat.x;
	float yz = quat.y * quat.z;
	float xw = quat.x * quat.w;

	array[0] = 1 - 2 * yy - 2 * zz;
	array[1] = 2 * xy + 2 * zw;
	array[2] = 2 * xz - 2 * yw;

	array[3] = 2 * xy - 2 * zw;
	array[4] = 1 - 2 * xx - 2 * zz;
	array[5] = 2 * yz + 2 * xw;

	array[6] = 2 * xz + 2 * yw;
	array[7] = 2 * yz - 2 * xw;
	array[8] = 1 - 2 * xx - 2 * yy;
}


Matrix3::~Matrix3(void)	{

}

Matrix3 Matrix3::Rotation(float degrees, const Vector3 &inaxis)	 {
	Matrix3 m;

	Vector3 axis = inaxis;

	axis.Normalise();

	float c = cos(Maths::DegreesToRadians(degrees));
	float s = sin(Maths::DegreesToRadians(degrees));

	m.array[0]  = (axis.x * axis.x) * (1.0f - c) + c;
	m.array[1]  = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.array[2]  = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.array[3]  = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.array[4]  = (axis.y * axis.y) * (1.0f - c) + c;
	m.array[5]  = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.array[6]  = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.array[7]  = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.array[8]  = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

Matrix3 Matrix3::Scale( const Vector3 &scale )	{
	Matrix3 m;

	m.array[0]  = scale.x;
	m.array[4]  = scale.y;
	m.array[8]  = scale.z;	

	return m;
}

void	Matrix3::ToZero()	{
	for(int i = 0; i < 9; ++i) {
		array[0] = 0.0f;
	}
}

//http://staff.city.ac.uk/~sbbh653/publications/euler.pdf
Vector3 Matrix3::ToEuler() const {
	//float h = (float)RadiansToDegrees(atan2(-values[6], values[0]));
	//float b = (float)RadiansToDegrees(atan2(-values[5], values[4]));
	//float a = (float)RadiansToDegrees(asin(values[3]));

	//return Vector3(a, h, b);

	//psi  = x;
	//theta = y;
	//phi = z



	float testVal = abs(array[2]) + 0.00001f;

	if (testVal < 1.0f) {
		float theta1 = -asin(array[2]);
		float theta2 = Maths::PI - theta1;

		float cost1 = cos(theta1);
		//float cost2 = cos(theta2);

		float psi1 = Maths::RadiansToDegrees(atan2(array[5] / cost1, array[8] / cost1));
		//float psi2 = Maths::RadiansToDegrees(atan2(array[5] / cost2, array[8] / cost2));

		float phi1 = Maths::RadiansToDegrees(atan2(array[1] / cost1, array[0] / cost1));
		//float phi2 = Maths::RadiansToDegrees(atan2(array[1] / cost2, array[0] / cost2));

		theta1 = Maths::RadiansToDegrees(theta1);
		//theta2 = Maths::RadiansToDegrees(theta2);

		return Vector3(psi1, theta1, phi1);
	}
	else {
		float phi	= 0.0f;	//x


		float theta = 0.0f;	//y
		float psi	= 0.0f;	//z

		float delta = atan2(array[3], array[6]);

		if (array[2] < 0.0f) {
			theta = Maths::PI / 2.0f;
			psi = phi + delta;
		}
		else {
			theta = -Maths::PI / 2.0f;
			psi = phi + delta;
		}

		return Vector3(Maths::RadiansToDegrees(psi), Maths::RadiansToDegrees(theta), Maths::RadiansToDegrees(phi));
	}

	//float sp = values[2];

	//sp = clamp(sp, -1.0f, 1.0f);



	//float theta = -asin(sp);
	//float cp = cos(theta);


	//Vector3 pyr;

	//if (cp > 0.01f) {
	//	pyr.x = RadiansToDegrees(theta);
	//	pyr.y = RadiansToDegrees(atan2(values[1], values[0]));
	//	pyr.z = RadiansToDegrees(atan2(values[5], values[8]));

	//}
	//else {
	//	pyr.x = RadiansToDegrees(theta);

	//	pyr.y = RadiansToDegrees(-atan2(values[3], values[4]));

	//	pyr.z = 0.0f;
	//}

	//return pyr;
}

Matrix3 Matrix3::FromEuler(const Vector3 &euler) {
	Matrix3 m;

	float heading	= Maths::DegreesToRadians(euler.y);
	float attitude	= Maths::DegreesToRadians(euler.x);
	float bank		= Maths::DegreesToRadians(euler.z);

	float ch = cos(heading);
	float sh = sin(heading);
	float ca = cos(attitude);
	float sa = sin(attitude);
	float cb = cos(bank);
	float sb = sin(bank);

	m.array[0] = ch * ca;
	m.array[3] = sh*sb - ch*sa*cb;
	m.array[6] = ch*sa*sb + sh*cb;
	m.array[1] = sa;
	m.array[4] = ca*cb;
	m.array[7] = -ca*sb;
	m.array[2] = -sh*ca;
	m.array[5] = sh*sa*cb + ch*sb;
	m.array[8] = -sh*sa*sb + ch*cb;

	return m;
}

Vector3 Matrix3::GetRow(unsigned int row) const {
	assert(row < 3);
	int start = row;
	return Vector3(
		array[start],
		array[start + 3],
		array[start + 6]
	);
}

void Matrix3::SetRow(unsigned int row, const Vector3 &val) {
	assert(row < 3);

	int start = 3 * row;

	array[start] = val.x;
	array[start + 3] = val.y;
	array[start + 6] = val.z;
}

Vector3 Matrix3::GetColumn(unsigned int column) const {
	assert(column < 3);
	int start = 3 * column;
	return Vector3(
		array[start],
		array[start + 1],
		array[start + 2]
	);
}

void Matrix3::SetColumn(unsigned int column, const Vector3 &val) {
	assert(column < 3);
	int start = 3 * column;
	array[start] = val.x;
	array[start + 1] = val.y;
	array[start + 2] = val.z;
}

Vector3 Matrix3::GetDiagonal() const {
	return Vector3(array[0], array[4], array[8]);
}

void	Matrix3::SetDiagonal(const Vector3 &in) {
	array[0] = in.x;
	array[4] = in.y;
	array[8] = in.z;
}

Vector3 Matrix3::operator*(const Vector3 &v) const {
	Vector3 vec;

	vec.x = v.x*array[0] + v.y*array[3] + v.z*array[6];
	vec.y = v.x*array[1] + v.y*array[4] + v.z*array[7];
	vec.z = v.x*array[2] + v.y*array[5] + v.z*array[8];

	return vec;
};