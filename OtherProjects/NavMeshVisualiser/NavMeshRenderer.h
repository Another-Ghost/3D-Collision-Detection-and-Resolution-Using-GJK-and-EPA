#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"

namespace NCL {
	class Camera;
	class MeshAnimation;

	class NavMeshRenderer : public OGLRenderer
	{
	public:
		NavMeshRenderer();
		virtual ~NavMeshRenderer();
		void Update(float dt) override;

		void DebugDrawSkeleton(const OGLMesh* mesh, const Matrix4* matrices);

	protected:
		void RenderFrame()	override;
		MeshAnimation* testAnim;
		OGLMesh*	navMesh;
		OGLShader*	navShader;
		OGLTexture* testTex;
		Camera*		camera;
		OGLShader* debugShader;
		int frame;

		float frameTime;
		float allTime;
	};
}

