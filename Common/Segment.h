#pragma once
#include "vector3.h"
namespace NCL {
	namespace Maths {
		struct Segment {
			//Why can't add constexpr before this?
			 Segment(const Vector3& pointA_, const Vector3& pointB_):pointA(pointA_),pointB(pointB_), middlePoint((pointA_+pointB_)/2),
				dir((pointB_-pointA_).Normalised()), length((pointB_ - pointA_).Length()){}

			Vector3 pointA;
			Vector3 pointB;
			Vector3 middlePoint;
			Vector3 dir;
			float length;
		};
	}
}

