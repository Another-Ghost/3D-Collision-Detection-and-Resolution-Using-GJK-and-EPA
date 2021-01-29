#include "VulkanPipelineBuilder.h"
#include "VulkanMesh.h"
#include "VulkanShader.h"

using namespace NCL;
using namespace Rendering;

VulkanPipelineBuilder::VulkanPipelineBuilder()	{
	dynamicStateEnables[0] = vk::DynamicState::eViewport;
	dynamicStateEnables[1] = vk::DynamicState::eScissor;

	dynamicCreate.setDynamicStateCount(2);
	dynamicCreate.setPDynamicStates(dynamicStateEnables);

	sampleCreate.setRasterizationSamples(vk::SampleCountFlagBits::e1);

	viewportCreate.setViewportCount(1);
	viewportCreate.setScissorCount(1);

	pipelineCreate.setPViewportState(&viewportCreate);

	depthStencilCreate.setDepthCompareOp(vk::CompareOp::eAlways)
		.setDepthTestEnable(false)
		.setDepthWriteEnable(false)
		.setStencilTestEnable(false)
		.setDepthBoundsTestEnable(false);

	//blendAttachState.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
	//	.setBlendEnable(false)
	//	.setAlphaBlendOp(vk::BlendOp::eAdd)
	//	.setColorBlendOp(vk::BlendOp::eAdd)
	//	.setSrcAlphaBlendFactor(vk::BlendFactor::eSrcAlpha)
	//	.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
	//	.setDstAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
	//	.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);

	//blendCreate.setAttachmentCount(1);
	//blendCreate.setPAttachments(&blendAttachState);

	rasterCreate.setCullMode(vk::CullModeFlagBits::eNone)
		.setPolygonMode(vk::PolygonMode::eFill)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setLineWidth(1.0f);
}

VulkanPipelineBuilder::~VulkanPipelineBuilder()
{
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithDepthState(vk::CompareOp op, bool depthEnabled, bool writeEnabled, bool stencilEnabled) {
	depthStencilCreate.setDepthCompareOp(op)
		.setDepthTestEnable(depthEnabled)
		.setDepthWriteEnable(writeEnabled)
		.setStencilTestEnable(stencilEnabled);
	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithBlendState(vk::BlendFactor srcState, vk::BlendFactor dstState, bool enabled) {
	vk::PipelineColorBlendAttachmentState pipeBlend;

	pipeBlend.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
		.setBlendEnable(enabled)
		.setAlphaBlendOp(vk::BlendOp::eAdd)
		.setColorBlendOp(vk::BlendOp::eAdd)

		.setSrcAlphaBlendFactor(srcState)
		.setSrcColorBlendFactor(srcState)

		.setDstAlphaBlendFactor(dstState)
		.setDstColorBlendFactor(dstState);

	blendAttachStates.emplace_back(pipeBlend);

	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithRaster(vk::CullModeFlagBits cullMode, vk::PolygonMode polyMode) {
	rasterCreate.setCullMode(cullMode).setPolygonMode(polyMode);
	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithVertexSpecification(VulkanVertexSpecification* mesh, vk::PrimitiveTopology topology) {
	pipelineCreate.setPVertexInputState(&mesh->vertexInfo);

	inputAsmCreate.setTopology(topology);
	
	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithShaderState(VulkanShader* shader) {
	shader->FillShaderStageCreateInfo(pipelineCreate);
	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithLayout(vk::PipelineLayout layout) {
	this->layout = layout;
	pipelineCreate.setLayout(layout);
	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithPushConstant(vk::PushConstantRange layout) {
	allPushConstants.emplace_back(layout);
	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithPass(vk::RenderPass& renderPass) {
	pipelineCreate.setRenderPass(renderPass);
	return *this;
}


VulkanPipelineBuilder& VulkanPipelineBuilder::WithDescriptorSetLayout(vk::DescriptorSetLayout layout) {
	allLayouts.emplace_back(layout);
	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::WithDebugName(const string& name) {
	debugName = name;
	return *this;
}

VulkanPipeline	VulkanPipelineBuilder::Build(VulkanRenderer& renderer) {	
	vk::PipelineLayoutCreateInfo pipeLayoutCreate = vk::PipelineLayoutCreateInfo()
		.setSetLayoutCount((uint32_t)allLayouts.size())
		.setPSetLayouts(allLayouts.data())
		.setPPushConstantRanges(allPushConstants.data())
		.setPushConstantRangeCount((uint32_t)allPushConstants.size());

	if (blendAttachStates.empty()) {
		WithBlendState(vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, false);
	}

	blendCreate.setAttachmentCount((uint32_t)blendAttachStates.size());
	blendCreate.setPAttachments(blendAttachStates.data());

	vk::PipelineLayout pipelineLayout = renderer.device.createPipelineLayout(pipeLayoutCreate);

	pipelineCreate.setPColorBlendState(&blendCreate)
		.setPDepthStencilState(&depthStencilCreate)
		.setPDynamicState(&dynamicCreate)
		.setPInputAssemblyState(&inputAsmCreate)
		.setPMultisampleState(&sampleCreate)
		.setPRasterizationState(&rasterCreate)
		.setLayout(pipelineLayout);

	VulkanPipeline output;
	output.layout	= pipelineLayout;
	output.pipeline			= renderer.device.createGraphicsPipeline(renderer.pipelineCache, pipelineCreate).value;

	if (!debugName.empty()) {
		renderer.SetDebugName(vk::ObjectType::ePipeline, (uint64_t)(VkPipeline)output.pipeline, debugName);
	}

	return output;
}