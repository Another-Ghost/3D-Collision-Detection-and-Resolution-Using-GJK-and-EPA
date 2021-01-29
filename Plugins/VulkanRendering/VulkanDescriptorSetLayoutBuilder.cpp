#include "VulkanDescriptorSetLayoutBuilder.h"
#include "VulkanMesh.h"
#include "VulkanShader.h"

using namespace NCL;
using namespace Rendering;

VulkanDescriptorSetLayoutBuilder& VulkanDescriptorSetLayoutBuilder::WithSamplers(unsigned int count, vk::ShaderStageFlags inShaders) {
	vk::DescriptorSetLayoutBinding binding = vk::DescriptorSetLayoutBinding()
		.setBinding((uint32_t)addedBindings.size())
		.setDescriptorCount(count)
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setStageFlags(inShaders);

	addedBindings.emplace_back(binding);

	return *this;
}

VulkanDescriptorSetLayoutBuilder& VulkanDescriptorSetLayoutBuilder::WithUniformBuffers(unsigned int count, vk::ShaderStageFlags inShaders) {
	vk::DescriptorSetLayoutBinding binding = vk::DescriptorSetLayoutBinding()
		.setBinding((uint32_t)addedBindings.size())
		.setDescriptorCount(count)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setStageFlags(inShaders);

	addedBindings.emplace_back(binding);
	return *this;
}

VulkanDescriptorSetLayoutBuilder& VulkanDescriptorSetLayoutBuilder::WithDebugName(const string& name) {
	debugName = name;
	return *this;
}

vk::DescriptorSetLayout VulkanDescriptorSetLayoutBuilder::Build(VulkanRenderer& renderer) {
	vk::DescriptorSetLayout outLayout = renderer.device.createDescriptorSetLayout(
		vk::DescriptorSetLayoutCreateInfo({}, (uint32_t)addedBindings.size(), addedBindings.data())
	);	
	renderer.SetDebugName(vk::ObjectType::eDescriptorSetLayout, (uint64_t)(VkDescriptorSetLayout)outLayout, debugName);
	return outLayout;
}