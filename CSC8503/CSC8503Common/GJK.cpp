#include "GJK.h"

#include "../../Common/Plane.h"
#include "../../Common/Maths.h"

#define GJK_MAX_NUM_ITERATIONS 64


bool NCL::GJKCalculation(GameObject* coll1, GameObject* coll2, CollisionDetection::CollisionInfo& collisionInfo)
{
	collisionInfo.a = coll1;
	collisionInfo.b = coll2;

	Vector3* mtv;

	Vector3 coll1Pos = coll1->GetTransform().GetPosition();
	Vector3 coll2Pos = coll2->GetTransform().GetPosition();


	Point a, b, c, d; //Simplex: just a set of points (a is always most recently added)
	Vector3 search_dir = coll1Pos - coll2Pos; //initial search direction between colliders

	 //Get initial point for simplex
	//Point c;
	CalculateSearchPoint(c, search_dir, coll1, coll2);
	search_dir = -c.p; //search in direction of origin

	//Get second point for a line segment simplex
	//Point b;
	CalculateSearchPoint(b, search_dir, coll1, coll2);

	if (Vector3::Dot(b.p, search_dir) < 0) {
		return false;
	}//we didn't reach the origin, won't enclose it

	search_dir = Vector3::Cross(Vector3::Cross(c.p - b.p, -b.p), c.p - b.p); //search perpendicular to line segment towards origin
	if (search_dir == Vector3(0, 0, 0)) { //origin is on this line segment
		//Apparently any normal search vector will do?
		search_dir = Vector3::Cross(c.p - b.p, Vector3(1, 0, 0)); //normal with x-axis
		if (search_dir == Vector3(0, 0, 0))
			search_dir = Vector3::Cross(c.p - b.p, Vector3(0, 0, -1)); //normal with z-axis
	}
	int simp_dim = 2; //simplex dimension

	for (int iterations = 0; iterations < GJK_MAX_NUM_ITERATIONS; iterations++)
	{
		//Point a;
		CalculateSearchPoint(a, search_dir, coll1, coll2);

		if (Vector3::Dot(a.p, search_dir) < 0) {
			return false;
		}//we didn't reach the origin, won't enclose it

		simp_dim++;
		if (simp_dim == 3) {
			update_simplex3(a, b, c, d, simp_dim, search_dir);
		}
		else if (update_simplex4(a, b, c, d, simp_dim, search_dir)) {
			EPA(a, b, c, d, coll1, coll2, collisionInfo);
			return true;
		}
	}//endfor

	return false;
}

void NCL::update_simplex3(Point& a, Point& b, Point& c, Point& d, int& simp_dim, Vector3& search_dir)
{
	/* Required winding order:
	   //  b
	   //  | \
	   //  |   \
	   //  |    a
	   //  |   /
	   //  | /
	   //  c
	   */
	Vector3 n = Vector3::Cross(b.p - a.p, c.p - a.p); //triangle's normal
	Vector3 AO = -a.p; //direction to origin

	//Determine which feature is closest to origin, make that the new simplex

	simp_dim = 2;
	if (Vector3::Dot(Vector3::Cross(b.p - a.p, n), AO) > 0) { //Closest to edge AB
		c = a;
		//simp_dim = 2;
		search_dir = Vector3::Cross(Vector3::Cross(b.p - a.p, AO), b.p - a.p);
		return;
	}
	if (Vector3::Dot(Vector3::Cross(n, c.p - a.p), AO) > 0) { //Closest to edge AC
		b = a;
		//simp_dim = 2;
		search_dir = Vector3::Cross(Vector3::Cross(c.p - a.p, AO), c.p - a.p);
		return;
	}

	simp_dim = 3;
	if (Vector3::Dot(n, AO) > 0) { //Above triangle
		d = c;
		c = b;
		b = a;
		//simp_dim = 3;
		search_dir = n;
		return;
	}
	//else //Below triangle
	d = b;
	b = a;
	//simp_dim = 3;
	search_dir = -n;
	return;
}

bool NCL::update_simplex4(Point& a, Point& b, Point& c, Point& d, int& simp_dim, Vector3& search_dir)
{
	// a is peak/tip of pyramid, BCD is the base (counterclockwise winding order)
	//We know a priori that origin is above BCD and below a

	//Get normals of three new faces
	Vector3 ABC = Vector3::Cross(b.p - a.p, c.p - a.p);
	Vector3 ACD = Vector3::Cross(c.p - a.p, d.p - a.p);
	Vector3 ADB = Vector3::Cross(d.p - a.p, b.p - a.p);

	Vector3 AO = -a.p; //dir to origin
	simp_dim = 3; //hoisting this just cause

	//Plane-test origin with 3 faces
	/*
	// Note: Kind of primitive approach used here; If origin is in front of a face, just use it as the new simplex.
	// We just go through the faces sequentially and exit at the first one which satisfies dot product. Not sure this
	// is optimal or if edges should be considered as possible simplices? Thinking this through in my head I feel like
	// this method is good enough. Makes no difference for AABBS, should test with more complex colliders.
	*/
	if (Vector3::Dot(ABC, AO) > 0) { //In front of ABC
		d = c;
		c = b;
		b = a;
		search_dir = ABC;
		return false;
	}

	if (Vector3::Dot(ACD, AO) > 0) { //In front of ACD
		b = a;
		search_dir = ACD;
		return false;
	}
	if (Vector3::Dot(ADB, AO) > 0) { //In front of ADB
		c = d;
		d = b;
		b = a;
		search_dir = ADB;
		return false;
	}

	//else inside tetrahedron; enclosed!
	return true;
}

//Expanding Polytope Algorithm
#define EPA_TOLERANCE 0.0001
#define EPA_MAX_NUM_FACES 64
#define EPA_MAX_NUM_LOOSE_EDGES 32
#define EPA_MAX_NUM_ITERATIONS 64
void NCL::EPA(Point& a, Point& b, Point& c, Point& d, GameObject* coll1, GameObject* coll2, CollisionDetection::CollisionInfo& collisionInfo)
{
	Point faces[EPA_MAX_NUM_FACES][4]; //Array of faces, each with 3 verts and a normal

	Vector3 VertexA[3];
	Vector3 VertexB[3];

	//Init with final simplex from GJK
	faces[0][0] = a;
	faces[0][1] = b;
	faces[0][2] = c;
	faces[0][3].p = (Vector3::Cross(b.p - a.p, c.p - a.p)).Normalised(); //ABC
	faces[1][0] = a;
	faces[1][1] = c;
	faces[1][2] = d;
	faces[1][3].p = (Vector3::Cross(c.p - a.p, d.p - a.p)).Normalised(); //ACD
	faces[2][0] = a;
	faces[2][1] = d;
	faces[2][2] = b;
	faces[2][3].p = (Vector3::Cross(d.p - a.p, b.p - a.p)).Normalised(); //ADB
	faces[3][0] = b;
	faces[3][1] = d;
	faces[3][2] = c;
	faces[3][3].p = (Vector3::Cross(d.p - b.p, c.p - b.p)).Normalised(); //BDC

	int num_faces = 4;
	int closest_face;

	for (int iterations = 0; iterations < EPA_MAX_NUM_ITERATIONS; iterations++) {
		//Find face that's closest to origin
		float min_dist = Vector3::Dot(faces[0][0].p, faces[0][3].p);
		closest_face = 0;
		for (int i = 1; i < num_faces; i++) {
			float dist = Vector3::Dot(faces[i][0].p, faces[i][3].p);
			if (dist < min_dist) {
				min_dist = dist;
				closest_face = i;
			}
		}

		//search normal to face that's closest to origin
		Vector3 search_dir = faces[closest_face][3].p;

		Point p;
		CalculateSearchPoint(p, search_dir, coll1, coll2);

		if (Vector3::Dot(p.p, search_dir) - min_dist < EPA_TOLERANCE) {

		/*Core of calculating collision information*/
			Plane closestPlane = Plane::PlaneFromTri(faces[closest_face][0].p, faces[closest_face][1].p, faces[closest_face][2].p); //plane of closest triangle face
			Vector3 projectionPoint = closestPlane.ProjectPointOntoPlane(Vector3(0, 0, 0)); //projecting the origin onto the triangle(both are in Minkowski space)
			float u, v, w;
			Barycentric(faces[closest_face][0].p, faces[closest_face][1].p, faces[closest_face][2].p,
				projectionPoint, u, v, w); //finding the barycentric coordinate of this projection point to the triangle

			//The contact points just have the same barycentric coordinate in their own triangles which  are composed by result coordinates of support function 
			Vector3 localA = faces[closest_face][0].a * u + faces[closest_face][1].a * v + faces[closest_face][2].a * w;
			Vector3 localB = faces[closest_face][0].b * u + faces[closest_face][1].b * v + faces[closest_face][2].b * w;
			float penetration = (localA - localB).Length();
			Vector3 normal = (localA - localB).Normalised();

			//Convergence (new point is not significantly further from origin)
			localA -= coll1->GetTransform().GetPosition();
			localB -= coll2->GetTransform().GetPosition();

			collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		/*Core of calculating collision information*/

			return;
		}

		Point loose_edges[EPA_MAX_NUM_LOOSE_EDGES][2]; //keep track of edges we need to fix after removing faces
		int num_loose_edges = 0;

		//Find all triangles that are facing p
		for (int i = 0; i < num_faces; i++)
		{
			if (Vector3::Dot(faces[i][3].p, p.p - faces[i][0].p) > 0) //triangle i faces p, remove it
			{
				//Add removed triangle's edges to loose edge list.
				//If it's already there, remove it (both triangles it belonged to are gone)
				for (int j = 0; j < 3; j++) //Three edges per face
				{
					Point current_edge[2] = { faces[i][j], faces[i][(j + 1) % 3] };
					bool found_edge = false;
					for (int k = 0; k < num_loose_edges; k++) //Check if current edge is already in list
					{
						if (loose_edges[k][1].p == current_edge[0].p && loose_edges[k][0].p == current_edge[1].p) {
							loose_edges[k][0] = loose_edges[num_loose_edges - 1][0]; //Overwrite current edge
							loose_edges[k][1] = loose_edges[num_loose_edges - 1][1]; //with last edge in list
							num_loose_edges--;
							found_edge = true;
							k = num_loose_edges; //exit loop because edge can only be shared once
						}
					}//endfor loose_edges

					if (!found_edge) { //add current edge to list
						// assert(num_loose_edges<EPA_MAX_NUM_LOOSE_EDGES);
						if (num_loose_edges >= EPA_MAX_NUM_LOOSE_EDGES) break;
						loose_edges[num_loose_edges][0] = current_edge[0];
						loose_edges[num_loose_edges][1] = current_edge[1];
						num_loose_edges++;
					}
				}

				//Remove triangle i from list
				faces[i][0] = faces[num_faces - 1][0];
				faces[i][1] = faces[num_faces - 1][1];
				faces[i][2] = faces[num_faces - 1][2];
				faces[i][3] = faces[num_faces - 1][3];
				num_faces--;
				i--;
			}//endif p can see triangle i
		}//endfor num_faces

		//Reconstruct polytope with p added
		for (int i = 0; i < num_loose_edges; i++)
		{
			// assert(num_faces<EPA_MAX_NUM_FACES);
			if (num_faces >= EPA_MAX_NUM_FACES) break;
			faces[num_faces][0] = loose_edges[i][0];
			faces[num_faces][1] = loose_edges[i][1];
			faces[num_faces][2] = p;
			faces[num_faces][3].p = Vector3::Cross(loose_edges[i][0].p - loose_edges[i][1].p, loose_edges[i][0].p - p.p).Normalised();

			//Check for wrong normal to maintain CCW winding
			float bias = 0.000001; //in case dot result is only slightly < 0 (because origin is on face)
			if (Vector3::Dot(faces[num_faces][0].p, faces[num_faces][3].p) + bias < 0) {
				Point temp = faces[num_faces][0];
				faces[num_faces][0] = faces[num_faces][1];
				faces[num_faces][1] = temp;
				faces[num_faces][3].p = -faces[num_faces][3].p;
			}
			num_faces++;
		}
	} //End for iterations
	printf("EPA did not converge\n");
	//Return most recent closest point
	Vector3 search_dir = faces[closest_face][3].p;

	Point p;
	CalculateSearchPoint(p, search_dir, coll1, coll2);

	Plane closestPlane = Plane::PlaneFromTri(faces[closest_face][0].p, faces[closest_face][1].p, faces[closest_face][2].p);
	Vector3 projectionPoint = closestPlane.ProjectPointOntoPlane(Vector3(0, 0, 0));
	float u, v, w;
	Barycentric(faces[closest_face][0].p, faces[closest_face][1].p, faces[closest_face][2].p,
		projectionPoint, u, v, w);
	Vector3 localA = faces[closest_face][0].a * u + faces[closest_face][1].a * v + faces[closest_face][2].a * w;
	Vector3 localB = faces[closest_face][0].b * u + faces[closest_face][1].b * v + faces[closest_face][2].b * w;
	float penetration = (localA - localB).Length();
	Vector3 normal = (localA - localB).Normalised();

	collisionInfo.AddContactPoint(localA, localB, normal, penetration);

	return;
}

void NCL::CalculateSearchPoint(Point& point, Vector3& search_dir, GameObject* coll1, GameObject* coll2)
{
	point.b = coll2->GetBoundingVolume()->Support(search_dir, coll2->GetTransform());
	point.a = coll1->GetBoundingVolume()->Support(-search_dir, coll1->GetTransform());
	point.p = point.b - point.a;
}
