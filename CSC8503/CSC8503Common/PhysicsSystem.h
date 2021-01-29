#pragma once
#include "../CSC8503Common/GameWorld.h"
#include <set>


namespace NCL {
	namespace CSC8503 {
		class TutorialGame;
		class PhysicsSystem	{
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void Clear();

			void Update(float dt);
			
			/*test*/
			void TestUpdate(float dt);

			bool bPhysics;
			/*test*/

			void UseGravity(bool state) {
				applyGravity = state;
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}

			void SetGravity(const Vector3& g);
		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();

			void UpdateObjectAABBs();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) const;

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;

			float staticCountMax;
			float staticMaxPosMagn;

			std::set<CollisionDetection::CollisionInfo> allBroadPhaseCollisions;


			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;

			TutorialGame* tutorialGame;
		public:
			void SetTutorialGame(TutorialGame* tutorialGame_) { tutorialGame = tutorialGame_; }

			void InitBroadPhase();
		};
	}
}

