#pragma once
#include "VulkanRenderer.h"

namespace NCL {
	namespace Rendering {
		class VulkanDescriptorSetLayoutBuilder
		{
		public:
			VulkanDescriptorSetLayoutBuilder()  {};
			~VulkanDescriptorSetLayoutBuilder() {};

			VulkanDescriptorSetLayoutBuilder& WithSamplers(unsigned int count, vk::ShaderStageFlags inShaders);
			VulkanDescriptorSetLayoutBuilder& WithUniformBuffers(unsigned int count, vk::ShaderStageFlags inShaders);

			VulkanDescriptorSetLayoutBuilder& WithDebugName(const string& name);

			vk::DescriptorSetLayout Build(VulkanRenderer& renderer );

		protected:
			string	debugName;
			vector< vk::DescriptorSetLayoutBinding> addedBindings;
		};
	}
}

