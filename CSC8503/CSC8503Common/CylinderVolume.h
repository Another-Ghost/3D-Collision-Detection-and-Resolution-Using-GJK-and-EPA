#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
namespace NCL {
    class CylinderVolume : public CollisionVolume
    {
	public:
		CylinderVolume(float halfHeight, float radius) {
			this->halfHeight = halfHeight;
			this->radius = radius;
			this->type = VolumeType::Cylinder;
		};
		~CylinderVolume() {

		}
		float GetRadius() const {
			return radius;
		}

		float GetHalfHeight() const {
			return halfHeight;
		}

		Vector3 Support(const Vector3& dir, const Transform& transform) {
			Vector3 localDir = transform.GetInvRotMatrix() * dir; //find support in model space

			Vector3 dir_xz = Vector3(localDir.x, 0, localDir.z);
			Vector3 result = dir_xz.Normalised() * radius;
			result.y = (localDir.y > 0) ? halfHeight : -halfHeight;

			return transform.GetRotMatrix() * result + transform.GetPosition(); //convert support to world space
		}


	protected:
		float radius;
		float halfHeight;

    };
}
