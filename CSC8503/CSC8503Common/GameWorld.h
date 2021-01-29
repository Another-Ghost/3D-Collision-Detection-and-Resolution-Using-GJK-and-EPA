#pragma once
#include <vector>
#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"

//#include "../../Plugins/OpenGLRendering/OGLMesh.h"

namespace NCL {
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class Constraint;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c, bool andDelete = false);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false) const;

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			void GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

		protected:
			std::vector<GameObject*> gameObjects;
			std::vector<Constraint*> constraints;

			Camera* mainCamera;

			bool	shuffleConstraints;
			bool	shuffleObjects;
			int		worldIDCounter;

		public:
			void AddDebugPoint(const Vector3& position, float radius = 0.05, const Vector4& colour = Vector4(1, 0, 0, 1), float time = 1.0f);

			void SetDebugMode(bool debug) { bDebugMode = debug; }
			bool DebugMode() { return bDebugMode; }

		protected:
			void UpdateDebugObject(float dt);

			struct DebugObject {
				DebugObject(GameObject* object_, float time_) :object(object_), time(time_) {}

				GameObject* object;
				float time;
			};

			vector<DebugObject*> debugObjectList;

			bool bDebugMode;
		};
	}
}

