#pragma once
#include "VulkanRenderer.h"

namespace NCL {
namespace Rendering {
	class VulkanPipelineBuilder
	{
	public:
		VulkanPipelineBuilder();
		~VulkanPipelineBuilder();

		VulkanPipelineBuilder& WithDepthState(vk::CompareOp op, bool depthEnabled, bool writeEnabled, bool stencilEnabled = false);

		VulkanPipelineBuilder& WithBlendState(vk::BlendFactor srcState, vk::BlendFactor dstState, bool enabled = true);

		VulkanPipelineBuilder& WithRaster(vk::CullModeFlagBits cullMode, vk::PolygonMode polyMode = vk::PolygonMode::eFill);

		VulkanPipelineBuilder& WithVertexSpecification(VulkanVertexSpecification* mesh, vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList);

		VulkanPipelineBuilder& WithShaderState(VulkanShader* shader);

		VulkanPipelineBuilder& WithLayout(vk::PipelineLayout layout);

		VulkanPipelineBuilder& WithPushConstant(vk::PushConstantRange layout);

		VulkanPipelineBuilder& WithDescriptorSetLayout(vk::DescriptorSetLayout layout);

		VulkanPipelineBuilder& WithPass(vk::RenderPass& renderPass);

		VulkanPipelineBuilder& WithDebugName(const string& name);

		VulkanPipeline	Build(VulkanRenderer& renderer);

	protected:
		vk::GraphicsPipelineCreateInfo				pipelineCreate;
		vk::PipelineCacheCreateInfo					cacheCreate;
		vk::PipelineInputAssemblyStateCreateInfo	inputAsmCreate;
		vk::PipelineRasterizationStateCreateInfo	rasterCreate;
		vk::PipelineColorBlendStateCreateInfo		blendCreate;
		vk::PipelineDepthStencilStateCreateInfo		depthStencilCreate;
		vk::PipelineViewportStateCreateInfo			viewportCreate;
		vk::PipelineMultisampleStateCreateInfo		sampleCreate;
		vk::PipelineDynamicStateCreateInfo			dynamicCreate;

		vk::PipelineLayout layout;

		vector< vk::PipelineColorBlendAttachmentState>			blendAttachStates;

		vk::DynamicState dynamicStateEnables[2];

		vector< vk::DescriptorSetLayout> allLayouts;
		vector< vk::PushConstantRange> allPushConstants;

		string debugName;
	};
}
}

