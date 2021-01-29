#include "NavMeshRenderer.h"
#include "../../Common/Assets.h"
#include "../../Common/Camera.h"

#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"
#include "../../Common/MeshAnimation.h"

#include <fstream>
#include <iostream>
using namespace NCL;

NavMeshRenderer::NavMeshRenderer() : OGLRenderer(*Window::GetWindow())	{
	navMesh = new OGLMesh();

	std::ifstream mapFile(Assets::DATADIR + "test.navmesh");

	int vCount = 0;
	int iCount = 0;

	mapFile >> vCount;
	mapFile >> iCount;

	vector<Vector3>			meshVerts;
	vector<unsigned int>	meshIndices;

	for (int i = 0; i < vCount; ++i) {
		Vector3 temp;
		mapFile >> temp.x;
		mapFile >> temp.y;
		mapFile >> temp.z;
		meshVerts.emplace_back(temp);
	}

	for (int i = 0; i < iCount; ++i) {
		unsigned int temp = -1;
		mapFile >> temp;
		meshIndices.emplace_back(temp);
	}

	struct TriNeighbours {
		int indices[3];
	};

	int numTris = iCount / 3;	//the indices describe n / 3 triangles
	vector< TriNeighbours> allNeighbours;
	//Each of these triangles will be sharing edges with some other triangles
	//so it has a maximum of 3 'neighbours', desribed by an index into n / 3 tris
	//if its a -1, then the edge is along the edge of the map...
	for (int i = 0; i < numTris; ++i) {
		TriNeighbours neighbours;
		mapFile >> neighbours.indices[0];
		mapFile >> neighbours.indices[1];
		mapFile >> neighbours.indices[2];
		allNeighbours.emplace_back(neighbours);
	}

	navMesh->SetVertexPositions(meshVerts);
	navMesh->SetVertexIndices(meshIndices);

	navMesh->UploadToGPU();

	testAnim = new MeshAnimation("Role_T.anm");
	//testAnim = new MeshAnimation("mannequinidle.anm");
	//testAnim = new MeshAnimation("mannequinbind.anm");
	navMesh = new OGLMesh("Role_T.msh");
	//navMesh = new OGLMesh("mannequinarms.msh");
	navMesh->UploadToGPU();
	//navMesh->SetPrimitiveType(NCL::GeometryPrimitive::Points);

	navShader = new OGLShader("SkelVert.glsl", "SkelFrag.glsl");

	debugShader = new OGLShader("BasicMatrixVert.glsl", "basicFrag.glsl");

	testTex = (OGLTexture*)OGLTexture::RGBATextureFromFilename("Monster_X_diffuse_Green.png");

	camera = new Camera();

	camera->SetNearPlane(1.0f);
	camera->SetFarPlane(1000.0f);
	camera->SetPosition(Vector3(0, 3, 10));

	frame		= 0;
	frameTime	= 0.0f;
	allTime		= 0.0f;
}

NavMeshRenderer::~NavMeshRenderer() {
	delete navMesh;
	delete navShader;
	delete camera;
	delete testAnim;
}

void NavMeshRenderer::Update(float dt) {
	camera->UpdateCamera(dt);

	allTime += dt;

	frameTime -= dt;

	if (frameTime < 0) {
		frameTime += 1.0f / 24.0f;
		frame++;
		frame = frame % testAnim->GetFrameCount();
	}
	//frame = 0;
}

void NavMeshRenderer::RenderFrame() {
	BindShader(navShader);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	float screenAspect = (float)currentWidth / (float)currentHeight;

	Matrix4 viewMatrix = camera->BuildViewMatrix();
	Matrix4 projMatrix = camera->BuildProjectionMatrix(screenAspect);
	Matrix4 modelMat = Matrix4();

	//modelMat = Matrix4::Rotation(-90.0f, Vector3(1, 0, 0)) *Matrix4::Scale(Vector3(10.1f, 10.1f, 10.1f));

	//modelMat = Matrix4::Rotation(allTime * 25, Vector3(1, 0, 0)) *Matrix4::Scale(Vector3(0.0001f, 0.0001f, 0.0001f));

	//modelMat = Matrix4::Scale(Vector3(0.1f, 0.1f, 0.1f));

	
	modelMat = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f));

	int projLocation	= glGetUniformLocation(navShader->GetProgramID(), "projMatrix");
	int viewLocation	= glGetUniformLocation(navShader->GetProgramID(), "viewMatrix");
	int modelLocation	= glGetUniformLocation(navShader->GetProgramID(), "modelMatrix");
	int hasTexLocation	= glGetUniformLocation(navShader->GetProgramID(), "hasTexture");


	int invBindLocation = glGetUniformLocation(navShader->GetProgramID(), "invBindPose");

	int jointsLocation		= glGetUniformLocation(navShader->GetProgramID(), "joints");
	int jointCountLocation	= glGetUniformLocation(navShader->GetProgramID(), "jointCount");

	glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMat);
	glUniformMatrix4fv(viewLocation , 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(projLocation , 1, false, (float*)&projMatrix);

	int jointCount = navMesh->GetJointCount();

	vector<Matrix4> joints = navMesh->GetBindPose();

	const Matrix4* frameJoints = testAnim->GetJointData(frame);

	vector<Matrix4> invBindPose = navMesh->GetInverseBindPose();
	vector<Matrix4> transformedMats;

	for (int i = 0; i < joints.size(); ++i) {
		Matrix4 m;
		m = frameJoints[i] * invBindPose[i];
		//m = joints[i];
		transformedMats.emplace_back(m);
	}

	glUniform1i(jointCountLocation, jointCount);
	glUniformMatrix4fv(jointsLocation, jointCount, false, (float*)transformedMats.data());

	glUniformMatrix4fv(invBindLocation, jointCount, false, (float*)invBindPose.data());

	glUniform1i(hasTexLocation, 0);

	BindMesh(navMesh);

	BindTextureToShader(testTex, "testTex", 0);

	DrawBoundMesh();

	//DebugDrawSkeleton(navMesh, navMesh->GetBindPose().data());

	DebugDrawSkeleton(navMesh, frameJoints);
}

void NavMeshRenderer::DebugDrawSkeleton(const OGLMesh* mesh, const Matrix4* matrices) {
	int matCount = mesh->GetBindPose().size();

	vector<Vector3> verts;
	vector<unsigned int> indices;

	for (int i = 0; i < matCount; ++i) {
		verts.emplace_back(matrices[i].GetPositionVector());
	}
	for (int i = 0; i < matCount; ++i) {
		indices.emplace_back(i);
		int p = mesh->GetJointParents()[i];
		if (i == -1) {
			indices.emplace_back(i);
		}
		else {
			indices.emplace_back(p);
		}
	}
	OGLMesh* m = new OGLMesh();
	m->SetVertexPositions(verts);
	m->SetVertexIndices(indices);
	m->SetPrimitiveType(NCL::GeometryPrimitive::Lines);
	m->UploadToGPU();

	BindShader(debugShader);
	glDisable(GL_DEPTH_TEST);
	int projLocation = glGetUniformLocation(debugShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(debugShader->GetProgramID(), "viewMatrix");
	int modelLocation = glGetUniformLocation(debugShader->GetProgramID(), "modelMatrix");
	float screenAspect = (float)currentWidth / (float)currentHeight;
	Matrix4 viewMatrix = camera->BuildViewMatrix();
	Matrix4 projMatrix = camera->BuildProjectionMatrix(screenAspect);
	Matrix4 modelMat = Matrix4();

	glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMat);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	BindMesh(m);
	DrawBoundMesh();
	BindMesh(nullptr);

	glEnable(GL_DEPTH_TEST);
	delete m;

}