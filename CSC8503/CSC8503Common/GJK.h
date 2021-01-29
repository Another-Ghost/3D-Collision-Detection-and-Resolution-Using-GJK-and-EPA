#pragma once

#include "GameObject.h"
#include "CollisionDetection.h"

namespace NCL {
	struct Point {
		Vector3 p; //Conserve Minkowski Difference
		Vector3 a; //Result coordinate of object A's support function 
		Vector3 b; //Result coordinate of object B's support function 
	};

	using namespace NCL::Maths;
	using namespace NCL::CSC8503;

	//Gilbert¨CJohnson¨CKeerthi distance algorithm
	bool GJKCalculation(GameObject* coll1, GameObject* coll2, CollisionDetection::CollisionInfo& collisionInfo); 

	//Internal functions used in the GJK algorithm
	void update_simplex3(Point& a, Point& b, Point& c, Point& d, int& simp_dim, Vector3& search_dir);
	bool update_simplex4(Point& a, Point& b, Point& c, Point& d, int& simp_dim, Vector3& search_dir);

	//Expanding Polytope Algorithm. 
	void EPA(Point& a, Point& b, Point& c, Point& d, GameObject* coll1, GameObject* coll2, CollisionDetection::CollisionInfo& collisionInfo);

	//Calculate the Minkowski Difference and conserve the the support function results at the same time.
	void CalculateSearchPoint(Point& point, Vector3& search_dir, GameObject* coll1, GameObject* coll2);
}