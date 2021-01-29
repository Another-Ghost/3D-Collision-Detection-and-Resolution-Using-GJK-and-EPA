/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Quaternion.h"
#include "Matrix4.h"
#include "Matrix3.h"
#include "Vector3.h"
#include "Maths.h"
#include <algorithm>
#include <cmath>

using namespace NCL;
using namespace NCL::Maths;

Quaternion::Quaternion(void)
{
	x = y = z = 0.0f;
	w = 1.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion::Quaternion(const Vector3& vector, float w) {
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
	this->w = w;
}

Quaternion::Quaternion(const Matrix4 &m) {
	w = sqrt(std::max(0.0f, (1.0f + m.array[0] + m.array[5] + m.array[10])))  * 0.5f;

	if (abs(w) < 0.0001f) {
		x = sqrt(std::max(0.0f, (1.0f + m.array[0] - m.array[5] - m.array[10]))) / 2.0f;
		y = sqrt(std::max(0.0f, (1.0f - m.array[0] + m.array[5] - m.array[10]))) / 2.0f;
		z = sqrt(std::max(0.0f, (1.0f - m.array[0] - m.array[5] + m.array[10]))) / 2.0f;

		x = (float)copysign(x, m.array[9] - m.array[6]);
		y = (float)copysign(y, m.array[2] - m.array[8]);
		z = (float)copysign(z, m.array[4] - m.array[1]);
	}
	else {
		float qrFour = 4.0f * w;
		float qrFourRecip = 1.0f / qrFour;

		x = (m.array[6] - m.array[9]) * qrFourRecip;
		y = (m.array[8] - m.array[2]) * qrFourRecip;
		z = (m.array[1] - m.array[4]) * qrFourRecip;
	}
}

Quaternion::Quaternion(const Matrix3 &m) {
	w = sqrt(std::max(0.0f, (1.0f + m.array[0] + m.array[4] + m.array[8]))) * 0.5f;

	float qrFour = 4.0f * w;
	float qrFourRecip = 1.0f / qrFour;

	x = (m.array[5] - m.array[7]) * qrFourRecip;
	y = (m.array[6] - m.array[2]) * qrFourRecip;
	z = (m.array[1] - m.array[3]) * qrFourRecip;
}

Quaternion::~Quaternion(void)
{
}

float Quaternion::Dot(const Quaternion &a,const Quaternion &b){
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

void Quaternion::Normalise(){
	float magnitude = sqrt(x*x + y*y + z*z + w*w);

	if(magnitude > 0.0f){
		float t = 1.0f / magnitude;

		x *= t;
		y *= t;
		z *= t;
		w *= t;
	}
}

void Quaternion::CalculateW()	{
	w = 1.0f - (x*x)-(y*y)-(z*z);
	if(w < 0.0f) {
		w = 0.0f;
	}
	else{
		w = - sqrt(w);
	}
}

Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x,-y,-z,w);
}

Quaternion Quaternion::Lerp(const Quaternion &from, const Quaternion &to, float by) {
	Quaternion temp = to;

	float dot = Quaternion::Dot(from,to);

	if(dot < 0.0f) {
		temp = -to;
	}

	return (from * (1.0f - by)) + (temp * by);
}

Quaternion Quaternion::Slerp(const Quaternion &from, const Quaternion &to, float by) {
	Quaternion temp = to;

	float dot = Quaternion::Dot(from,to);

	if(dot < 0.0f) {
		temp = -to;
	}

	return (from * (cos(by))) + (to * (1.0f - cos(by)));
}

//http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
//Verified! Different values to above, due to difference between x/z being 'forward'
Vector3 Quaternion::ToEuler() const {
	Vector3 euler;

	float t = x*y + z*w;

	if (t > 0.4999) {
		euler.z = Maths::RadiansToDegrees(Maths::PI / 2.0f);
		euler.y = Maths::RadiansToDegrees(2.0f * atan2(x, w));
		euler.x = 0.0f;

		return euler;
	}

	if (t < -0.4999) {
		euler.z = -Maths::RadiansToDegrees(Maths::PI / 2.0f);
		euler.y = -Maths::RadiansToDegrees(2.0f * atan2(x, w));
		euler.x = 0.0f;
		return euler;
	}

	float sqx = x*x;
	float sqy = y*y;
	float sqz = z*z;

	euler.z = Maths::RadiansToDegrees(asin(2 * t));
	euler.y = Maths::RadiansToDegrees(atan2(2 * y*w - 2 * x*z, 1.0f - 2 * sqy - 2 * sqz));
	euler.x = Maths::RadiansToDegrees(atan2(2 * x*w - 2 * y*z, 1.0f - 2 * sqx - 2.0f*sqz));

	return euler;
}

//http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/
//VERIFIED AS CORRECT - Pitch and roll are changed around as the above uses x as 'forward', whereas we use -z
Quaternion Quaternion::EulerAnglesToQuaternion(float roll, float yaw, float pitch) {
	float cos1 = (float)cos(Maths::DegreesToRadians(yaw   * 0.5f));
	float cos2 = (float)cos(Maths::DegreesToRadians(pitch * 0.5f));
	float cos3 = (float)cos(Maths::DegreesToRadians(roll  * 0.5f));

	float sin1 = (float)sin(Maths::DegreesToRadians(yaw   * 0.5f));
	float sin2 = (float)sin(Maths::DegreesToRadians(pitch * 0.5f));
	float sin3 = (float)sin(Maths::DegreesToRadians(roll  * 0.5f));

	Quaternion q;

	q.x = (sin1 * sin2 * cos3) + (cos1 * cos2 * sin3);
	q.y = (sin1 * cos2 * cos3) + (cos1 * sin2 * sin3);
	q.z = (cos1 * sin2 * cos3) - (sin1 * cos2 * sin3);
	q.w = (cos1 * cos2 * cos3) - (sin1 * sin2 * sin3);

	return q;
};

Quaternion Quaternion::AxisAngleToQuaterion(const Vector3& vector, float degrees) {
	float theta		= (float)Maths::DegreesToRadians(degrees);
	float result	= (float)sin(theta / 2.0f);

	return Quaternion((float)(vector.x * result), (float)(vector.y * result), (float)(vector.z * result), (float)cos(theta / 2.0f));
}


Vector3		Quaternion::operator *(const Vector3 &a)	const {
	Quaternion newVec = *this * Quaternion(a.x, a.y, a.z, 0.0f) * Conjugate();
	return Vector3(newVec.x, newVec.y, newVec.z);
}