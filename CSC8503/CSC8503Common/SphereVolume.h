#pragma once
#include "CollisionVolume.h"

#include "../CSC8503Common/CollisionDetection.h"
#include "../CSC8503Common/Ray.h"


namespace NCL {
	using namespace NCL;
	using namespace NCL::CSC8503;
	using namespace CSC8503;

	class SphereVolume : CollisionVolume
	{
	public:
		SphereVolume(float sphereRadius = 1.0f) {
			type	= VolumeType::Sphere;
			radius	= sphereRadius;
		}
		~SphereVolume() {}

		float GetRadius() const {
			return radius;
		}

		Vector3 Support(const Vector3& dir, const Transform& transform) {
			return dir.Normalised() * radius + transform.GetPosition();

			//Ray r(Vector3(0, 0, 0), dir);
			//RayCollision collision;
			//CollisionDetection::RaySphereIntersection(r, transform, *this, collision);
			//return collision.collidedAt;
		}

	protected:
		float	radius;
	};
}

