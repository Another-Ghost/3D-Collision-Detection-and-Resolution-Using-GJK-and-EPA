#pragma once
#include "CollisionVolume.h"

#include "../CSC8503Common/CollisionDetection.h"
#include "../CSC8503Common/Ray.h"
//#include "CollisionDetection.h"

namespace NCL {
	using namespace NCL;
	using namespace NCL::CSC8503;
	using namespace CSC8503;

    class CapsuleVolume : public CollisionVolume
    {
    public:
        CapsuleVolume(float halfHeight, float radius) {
            this->halfHeight    = halfHeight;
            this->radius        = radius;
            this->type          = VolumeType::Capsule;
        };
        ~CapsuleVolume() {

        }
        float GetRadius() const {
            return radius;
        }

        float GetHalfHeight() const {
            return halfHeight;
        }

		Vector3 Support(const Vector3& dir, const Transform& transform) {
			Vector3 localDir = transform.GetInvRotMatrix() * dir; //find support in model space

			//Vector3 dir_xz = Vector3(localDir.x, 0, localDir.z);
   //         const float dy = Vector3::Dot(dir, Vector3(0, 1, 0));
   //         if (abs(dy) > 0.9999f) {
   //             return (dy > 0.0f ?
   //                 transform.GetRotMatrix() * Vector3(0, halfHeight - radius, 0) + transform.GetPosition()
   //                 : transform.GetRotMatrix() * Vector3(0, -(halfHeight - radius), 0) + transform.GetPosition());
   //         }
			//else if (abs(dy) < 0.0001f) // perpendicular
			//{
   //             return transform.GetRotMatrix() * (Vector3(0, 0, 0) - dir * radius) + transform.GetPosition();
			//}


			Vector3 result = localDir.Normalised() * radius;
			result.y += (localDir.y > 0) ? halfHeight-radius : -(halfHeight - radius);

			return transform.GetRotMatrix() * result + transform.GetPosition(); //convert support to world space

			//Ray r(Vector3(0, 0, 0), dir);
			//RayCollision collision;
			//CollisionDetection::RayCapsuleIntersection(r, transform, *this, collision);
			//return collision.collidedAt;

		}

    protected:
        float radius;
        float halfHeight;
    };
}

