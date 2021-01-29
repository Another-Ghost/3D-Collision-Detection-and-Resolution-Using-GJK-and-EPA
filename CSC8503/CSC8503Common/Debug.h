#pragma once
#include "../../Plugins/OpenGLRendering/OGLRenderer.h"
#include <vector>
#include <string>

namespace NCL {
	class Debug
	{
	public:
		static void Print(const std::string& text, const Vector2&pos, const Vector4& colour = Vector4(1, 1, 1, 1));
		static void DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour = Vector4(1, 1, 1, 1), float time = 0.0f);

		static void DrawAxisLines(const Matrix4 &modelMatrix, float scaleBoost = 1.0f, float time = 0.0f);

		static void SetRenderer(OGLRenderer* r) {
			renderer = r;
		}

		static void FlushRenderables(float dt);


		static const Vector4 RED;
		static const Vector4 GREEN;
		static const Vector4 BLUE;

		static const Vector4 BLACK;
		static const Vector4 WHITE;

		static const Vector4 YELLOW;
		static const Vector4 MAGENTA;
		static const Vector4 CYAN;

	protected:
		struct DebugStringEntry {
			std::string	data;
			Vector2 position;
			Vector4 colour;
		};

		struct DebugLineEntry {
			Vector3 start;
			Vector3 end;
			float	time;
			Vector4 colour;
		};

		Debug() {}
		~Debug() {}

		static std::vector<DebugStringEntry>	stringEntries;
		static std::vector<DebugLineEntry>	lineEntries;

		static OGLRenderer* renderer;
	};
}

