#pragma once

#include "../../Common/Camera.h"
#include "../../Common/Plane.h"

#include "../../Common/Segment.h"

#include "Transform.h"
#include "GameObject.h"

#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "CapsuleVolume.h"

#include "CylinderVolume.h"

#include "Ray.h"

using NCL::Camera;
using namespace NCL::Maths;
using namespace NCL::CSC8503;
namespace NCL {
	class CollisionDetection
	{
	public:
		struct ContactPoint {
			Vector3 localA;
			Vector3 localB;
			Vector3 normal;
			float	penetration;
		};
		struct CollisionInfo {
			GameObject* a;
			GameObject* b;		
			mutable int		framesLeft; //? 
			mutable int staticCount;

			ContactPoint point;

			void AddContactPoint(const Vector3& localA, const Vector3& localB, const Vector3& normal, float p) {
				point.localA		= localA;
				point.localB		= localB;
				point.normal		= normal;
				point.penetration	= p;
			}


			//Advanced collision detection / resolution
			bool operator < (const CollisionInfo& other) const {
				size_t otherHash = (size_t)other.a->GetWorldID() + ((size_t)other.b->GetWorldID() << 32);
				size_t thisHash  = (size_t)a->GetWorldID()		 + ((size_t)b-> GetWorldID() << 32);

				if (thisHash < otherHash) {
					return true;
				}
				return false;
			}

			bool operator ==(const CollisionInfo& other) const {
				if (other.a == a && other.b == b) {
					return true;
				}
				return false;
			}
		};



		//TODO ADD THIS PROPERLY
		static bool RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision);

		static Ray BuildRayFromMouse(const Camera& c);

		static bool RayIntersection(const Ray&r, GameObject& object, RayCollision &collisions);

		static bool RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume&	volume, RayCollision& collision);
		static bool RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume&	volume, RayCollision& collision);
		
		static bool RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision);
		static bool RaySphereIntersection(const Ray& r, const Vector3& spherePos, const float& sphereRadius, RayCollision& collision);
		
		static bool RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision);

		static bool SegmentCapsuleIntersection(const Segment& seg, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision);

		/*May change to a new class*/
		//static float LineToLineDistance(const Vector3& a1, const Vector3& a2, const Vector3& b1, const Vector3& b2);
		//static float LineToLineDistance(const Vector3& a1, const Vector3& a2, const Vector3& b1, const Vector3& b2, Vector3& a, Vector3& b);
		//static float PointToLineDistance(const Vector3& p1, const Vector3& p2, const Vector3& p);
		/*May change to a new class*/

		static bool RayCylinderIntersection(const Ray& r, const Transform& worldTransform, const CylinderVolume& volume, RayCollision& collision);

		static bool RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions);

		static bool	AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB);


		static bool ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo);


		static bool AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
										const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
										const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool AABBSphereIntersection(const AABBVolume& volumeA	 , const Transform& worldTransformA,
										const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool OBBIntersection(const OBBVolume& volumeA, const Transform& worldTransformA,
										const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool SphereCapsuleIntersection(
			const CapsuleVolume& volumeA, const Transform& worldTransformA,
			const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA,
			const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static bool MovingSphereAABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
			const SphereVolume& volumeS, const Transform& worldTransformS, const Vector3& movingDir, CollisionInfo& collisionInfo);

		static bool AABBCapsuleIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
			const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo);

		static Vector3 BoxCorner(const AABBVolume& volume, const Transform& worldTransform, int n);

		static Vector3 Unproject(const Vector3& screenPos, const Camera& cam);

		static Vector3		UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c);
		static Matrix4		GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane);
		static Matrix4		GenerateInverseView(const Camera &c);

		static bool SATTest(GameObject* coll1, GameObject* coll2, CollisionDetection::CollisionInfo& collisionInfo);

	protected:
	
	private:
		CollisionDetection()	{}
		~CollisionDetection()	{}
	};
}

