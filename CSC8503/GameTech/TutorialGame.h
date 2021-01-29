#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"

namespace NCL {
	namespace CSC8503 {

		class BonusObject;
		class Pawn;

		class TutorialGame {
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void UpdateKeys();


			void UpdateObjectKeys(float dt);


			void InitialiseAssets();

			void InitCamera();

			void InitWorld();


			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float height);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();


			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 5.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 5.0f, bool bStatic = false, float elasticity = 0.8f);
			
			/**/
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 5.0f);
			GameObject* AddCylinderToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 5.0f);


			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;


			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			/**/
			OGLMesh* cylinderMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 35, -10);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}
			

		protected:

			bool bDebugMode;

		public:
			GameWorld* GetWorld() { return world; }


		};
	}
}

