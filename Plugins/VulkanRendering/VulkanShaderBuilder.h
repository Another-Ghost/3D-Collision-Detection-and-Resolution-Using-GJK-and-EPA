#pragma once
#include "VulkanShader.h"
#include "VulkanRenderer.h"

namespace NCL {
	namespace Rendering {
		class VulkanShaderBuilder {
		public:
			VulkanShaderBuilder()	{};
			~VulkanShaderBuilder()	{};

			VulkanShaderBuilder& WithVertexBinary(const string& name);
			VulkanShaderBuilder& WithFragmentBinary(const string& name);
			VulkanShaderBuilder& WithGeometryBinary(const string& name);
			VulkanShaderBuilder& WithTessControlBinary(const string& name);
			VulkanShaderBuilder& WithTessEvalBinary(const string& name);

			VulkanShaderBuilder& WithDebugName(const string& name);

			VulkanShader*	Build(VulkanRenderer& renderer);

		protected:
			string shaderFiles[(int)ShaderStages::SHADER_MAX];
			string debugName;
		};
	}
}