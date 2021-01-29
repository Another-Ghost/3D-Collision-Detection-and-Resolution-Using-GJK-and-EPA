#pragma once

namespace NCL {
	namespace CSC8503 {
		class Constraint	{
		public:
			Constraint() {}
			virtual ~Constraint() {}

			virtual void UpdateConstraint(float dt) = 0;
		};
	}
}