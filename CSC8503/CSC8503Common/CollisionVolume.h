#pragma once

//#include "../../Common/Vector3.h"

//#include "../../Common/Matrix4.h"

#include "../CSC8503Common/Transform.h"
//#include "../CSC8503Common/GameObject.h"
//#include "../CSC8503Common/CollisionDetection.h"

namespace NCL {

	//using namespace NCL::Maths;
	using namespace NCL::CSC8503;


	enum class VolumeType {
		AABB	= 1, //Clever
		OBB		= 2,
		Sphere	= 4, 
		Mesh	= 8,
		Capsule = 16,
		Compound= 32,
		Cylinder= 64,
		Invalid = 256
	};

	class CollisionVolume
	{
	public:
		CollisionVolume() {
			type = VolumeType::Invalid;
		}
		~CollisionVolume() {}

		VolumeType type;

		//add a pointer to Gameobject
		//GameObject* 

		virtual Vector3 Support(const Vector3& dir, const Transform& transform) = 0;

		
	};
}