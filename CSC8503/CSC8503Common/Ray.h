#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Plane.h"

namespace NCL {
	namespace Maths {
		struct RayCollision {
			void*		node;			//Node that was hit
			Vector3		collidedAt;		//WORLD SPACE position of the collision!
			float		rayDistance;

			RayCollision() {
				node			= nullptr;
				rayDistance		= FLT_MAX;
			}
		};

		class Ray {
		public:
			Ray(Vector3 position, Vector3 direction) {
				this->position  = position;
				this->direction = direction;
			}
			~Ray(void) {}

			Vector3 GetPosition() const {return position;	}

			Vector3 GetDirection() const {return direction;	}

		protected:
			Vector3 position;	//World space position
			Vector3 direction;	//Normalised world space direction
		};
	}
}