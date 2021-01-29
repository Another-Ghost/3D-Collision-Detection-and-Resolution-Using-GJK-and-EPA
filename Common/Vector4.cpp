/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
using namespace NCL;
using namespace Maths;

Vector4::Vector4(const Vector3& v3, float newW) : x(v3.x), y(v3.y), z(v3.z), w (newW)  {

}

Vector4::Vector4(const Vector2& v2, float newZ, float newW) : x(v2.x), y(v2.y), z(newZ), w(newW) {

}