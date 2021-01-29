#pragma once
#include "CollisionVolume.h"
//#include "../../Common/Vector3.h"

#include "../CSC8503Common/CollisionDetection.h"
#include "../CSC8503Common/Ray.h"
//#include "CollisionDetection.h"

namespace NCL {
	using namespace NCL;
	using namespace NCL::CSC8503;
	using namespace CSC8503;


	class OBBVolume : CollisionVolume
	{
	public:
		OBBVolume(const Maths::Vector3& halfDims) {
			type		= VolumeType::OBB;
			halfSizes	= halfDims;
		}
		~OBBVolume() {}

		Maths::Vector3 GetHalfDimensions() const {
			return halfSizes;
		}

		Vector3 Support(const Vector3& dir, const Transform& transform) {
			Vector3 localDir = transform.GetInvRotMatrix() * dir; //find support in model space

			Vector3 result;
			result.x = (localDir.x > 0) ? halfSizes.x : -halfSizes.x;
			result.y = (localDir.y > 0) ? halfSizes.y : -halfSizes.y;
			result.z = (localDir.z > 0) ? halfSizes.z : -halfSizes.z;

			return transform.GetRotMatrix() * result + transform.GetPosition(); //convert support to world space
			//Ray r(Vector3(0, 0, 0), dir);
			//RayCollision collision;

			//CollisionDetection::RayOBBIntersection(r, transform, *this, collision);
			//return collision.collidedAt;
		}
	protected:
		Maths::Vector3 halfSizes;
	};
}

