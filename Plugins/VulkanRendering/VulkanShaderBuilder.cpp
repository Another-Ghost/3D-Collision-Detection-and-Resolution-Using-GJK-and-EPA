#include "VulkanShaderBuilder.h"

using namespace NCL;
using namespace Rendering;

VulkanShaderBuilder& VulkanShaderBuilder::WithVertexBinary(const string& name) {
	shaderFiles[(int)ShaderStages::SHADER_VERTEX] = name;
	return *this;
}

VulkanShaderBuilder& VulkanShaderBuilder::WithFragmentBinary(const string& name) {
	shaderFiles[(int)ShaderStages::SHADER_FRAGMENT] = name;
	return *this;
}

VulkanShaderBuilder& VulkanShaderBuilder::WithGeometryBinary(const string& name) {
	shaderFiles[(int)ShaderStages::SHADER_GEOMETRY] = name;
	return *this;
}

VulkanShaderBuilder& VulkanShaderBuilder::WithTessControlBinary(const string& name) {
	shaderFiles[(int)ShaderStages::SHADER_DOMAIN] = name;
	return *this;
}

VulkanShaderBuilder& VulkanShaderBuilder::WithTessEvalBinary(const string& name) {
	shaderFiles[(int)ShaderStages::SHADER_HULL] = name;
	return *this;
}

VulkanShaderBuilder& VulkanShaderBuilder::WithDebugName(const string& name) {
	debugName = name;
	return *this;
}

VulkanShader* VulkanShaderBuilder::Build(VulkanRenderer& renderer) {
	VulkanShader* newShader = new VulkanShader();

	newShader->SetSourceDevice(renderer.device);

	for (int i = 0; i < (int)ShaderStages::SHADER_MAX; ++i) {
		if (!shaderFiles[i].empty()) {
			newShader->AddBinaryShaderModule(shaderFiles[i],(ShaderStages)i);

			if (!debugName.empty()) {
				renderer.SetDebugName(vk::ObjectType::eShaderModule, (uint64_t)newShader->shaderModules[i].operator VkShaderModule() , debugName);
			}
		}
	};
	newShader->Init();
	return newShader;
}