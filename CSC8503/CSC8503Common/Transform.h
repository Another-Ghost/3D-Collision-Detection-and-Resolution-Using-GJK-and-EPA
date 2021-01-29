#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/Matrix3.h"
#include "../../Common/Vector3.h"
#include "../../Common/Quaternion.h"

#include <vector>

using std::vector;

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class Transform
		{
		public:
			Transform();
			~Transform();

			Transform& SetPosition(const Vector3& worldPos);
			Transform& SetScale(const Vector3& worldScale);
			Transform& SetOrientation(const Quaternion& newOr);

			Vector3 GetPosition() const {
				return position;
			}

			Vector3 GetScale() const {
				return scale;
			}

			Quaternion GetOrientation() const {
				return orientation;
			}

			Matrix4 GetMatrix() const {
				return matrix;
			}


			Matrix3 GetRotMatrix() const {
				return Matrix3(orientation);
			}

			Matrix3 GetInvRotMatrix() const {
				return Matrix3(orientation.Conjugate());
			}

			void UpdateMatrix();
		protected:
			Matrix4		matrix;
			Quaternion	orientation;
			Vector3		position;

			Vector3		scale;
		};
	}
}

