#include "SATAlgorithm.h"
#include "Debug.h"
using namespace NCL;
#include "Transform.h"

using namespace Maths;
using namespace CSC8503;

SATAlgorithm::SATAlgorithm()
{
}


SATAlgorithm::~SATAlgorithm()
{
}

bool SATAlgorithm::BoundingBoxSAT(const NCL::OBBVolume& volumeA, const Transform& worldTransformA,
	const NCL::OBBVolume& volumeB, const Transform& worldTransformB, CollisionDetection::CollisionInfo& collisionInfo
) {
	Vector3 aBoxSize = volumeA.GetHalfDimensions();// * 0.5f;
	Vector3 bBoxSize = volumeB.GetHalfDimensions();// * 0.5f;

	Matrix3 aInvOrientation = Matrix3(worldTransformA.GetOrientation().Conjugate());

	Vector3 bRelativePos = aInvOrientation * (worldTransformB.GetPosition() - worldTransformA.GetPosition());

	Matrix3 relativeOrientation = aInvOrientation * Matrix3(worldTransformB.GetOrientation());
	Matrix3 absoluteRelative = relativeOrientation.Absolute();

	//Vector3 bBoxExpanded = absoluteRelative * bBoxSize; //need to see how to use this better

	Vector3 realRelativeSize = relativeOrientation * bBoxSize;

	//box a can now be thought of as being axis aligned to b

	bool isParallel = false;

	for (int i = 0; i < 9; ++i) {
		if (absoluteRelative.array[i] > 0.999f) {
			isParallel = true;
			break;
		}
	}

	float bestOnA = -FLT_MAX;
	float bestOnB = -FLT_MAX;
	float bestOnEdge = -FLT_MAX;

	int bestAAxis = 0;
	int bestBAxis = 0;

	bool noCollide = false;

	//Test A axes
	for (int i = 0; i < 3; ++i) {
		float s = abs(bRelativePos[i]) - (aBoxSize[i] + Vector3::Dot(absoluteRelative.GetRow(i), bBoxSize));

		if (s > 0.0f) {
			noCollide = true;
			//definately not colliding, there's a separation on this axis
		}

		if (s > bestOnA) {
			bestOnA = s;
			bestAAxis = i;
		}
	}

	//Now test B Axes
	for (int i = 0; i < 3; ++i) {

		float s = abs(Vector3::Dot(bRelativePos, relativeOrientation.GetColumn(i))) - (bBoxSize[i] + Vector3::Dot(absoluteRelative.GetColumn(i), aBoxSize));

		if (s > 0.0f) {
			noCollide = true;
			//definately not colliding, there's a separation on this axis
		}

		if (s > bestOnB) {
			bestOnB = s;
			bestBAxis = i;
		}
	}

	//Now we have to also check the edges

	Vector3 bestEdgeAxis;
	if (!isParallel) {
		for (int a = 0; a < 3; ++a) {
			Vector3 aDir = absoluteRelative.GetColumn(a);
			for (int b = 0; b < 3; ++b) {
				Vector3 bDir = relativeOrientation.GetColumn(b);

				Vector3 l = Vector3::Cross(aDir, bDir);

				if (l.Length() == 0.0f) {
					continue;
				}

				float tl = Vector3::Dot(bRelativePos, l);
				float al = Vector3::Dot(aBoxSize, l);
				float bl = Vector3::Dot(realRelativeSize, l);

				float s = abs(tl) - (abs(al) + abs(bl));

				if (s > 0.0f) {
					noCollide = true;//definately not colliding, there's a separation on this axis
				}
				if (s > bestOnEdge) {
					bestOnEdge = s;
					//bestAAxis	= a;
					//bestBAxis	= b;
					bestEdgeAxis = l;
				}
			}
		}
	}

	Vector3 aNorm = Vector3(worldTransformA.GetMatrix().GetColumn(bestAAxis));
	Vector3 otherA = aInvOrientation.GetColumn(bestAAxis);

	Vector3 bNorm = Vector3(worldTransformB.GetMatrix().GetColumn(bestBAxis));
	Vector3 edgeNorm = bestEdgeAxis;

	Debug::DrawLine(worldTransformA.GetPosition(), worldTransformA.GetPosition() + (aNorm  * bestOnA * 0.5f), Vector4(1, 0, 0, 1));
	Debug::DrawLine(worldTransformA.GetPosition(), worldTransformA.GetPosition() + (otherA  * bestOnA * 0.5f), Vector4(0.5, 0, 0, 1));

	Debug::DrawLine(worldTransformB.GetPosition(), worldTransformB.GetPosition() + (bNorm  * bestOnB * 0.5f), Vector4(1, 1, 0, 1));


	//float bestFace = max(bestOnA, bestOnB);

	//if (bestOnEdge > bestFace) {
	//	Vector3 norm = bestEdgeAxis;
	//	Debug::DrawLine(worldTransformA.GetWorldPosition(), worldTransformA.GetWorldPosition() + (norm  * 15.0f), Vector4(1, 0, 0, 1));
	//}
	//else {
	//	if (bestOnA > bestOnB) {
	//		Vector3 norm = worldTransformA.GetWorldMatrix().GetColumn(bestAAxis).ToVector3();
	//		Debug::DrawLine(worldTransformA.GetWorldPosition(), worldTransformA.GetWorldPosition() + (norm  * 15.0f), Vector4(1, 0, 0, 1));
	//	}
	//	else {
	//		Vector3 bNorm = worldTransformB.GetWorldMatrix().GetColumn(bestBAxis).ToVector3();
	//		Debug::DrawLine(worldTransformB.GetWorldPosition(), worldTransformB.GetWorldPosition() + (bNorm  * 15.0f), Vector4(1, 0, 0, 1));
	//	}
	//}

	////Vector3 aNorm = worldTransformA.GetWorldMatrix().GetColumn(bestAAxis).ToVector3();
	////
	////Vector3 edgeNorm = bestEdgeAxis;

	////Vector3 lolTest = Vector3::Cross(aNorm, bNorm);
	//Vector3 bNorm = worldTransformB.GetWorldMatrix().GetColumn(bestBAxis).ToVector3();
	////Debug::DrawLine(worldTransformA.GetWorldPosition(), worldTransformA.GetWorldPosition() + (aNorm  * 15.0f), Vector4(1, 0, 0, 1));
	//Debug::DrawLine(worldTransformB.GetWorldPosition(), worldTransformB.GetWorldPosition() + (bNorm  * 15.0f), Vector4(1, 1, 0, 1));

	////Debug::DrawLine(worldTransformA.GetWorldPosition(), worldTransformA.GetWorldPosition() + (lolTest  * 15.0f), Vector4(0.2, 0.1, 0.8, 1));

	//////Debug::DrawLine(worldTransformA.GetWorldPosition(), worldTransformA.GetWorldPosition() + (edgeNorm  * 15.0f), Vector4(0, 1, 1, 1));


	if (noCollide) {
		return false;
	}

	float bestFace = max(bestOnA, bestOnB);

	//if (noCollide) {
	//	std::cout << "SAT NO COLLIDE?" << std::endl;
	//}

	if (bestOnEdge > bestFace) {
		std::cout << "SAT EDGE COLLISION " << bestOnEdge << std::endl;
	}
	else {
		std::cout << "SAT FACE COLLISION " << bestFace << std::endl;
	}
	return true;
}

void SATAlgorithm::OBBSupport(Vector3& min, Vector3& max, const Vector3& objectPos, const Vector3& axis) {
	Vector3 tangents[2];

	//tangents[0] = 

}