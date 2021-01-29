#include "VulkanRenderPassBuilder.h"

using namespace NCL;
using namespace Rendering;

VulkanRenderPassBuilder::VulkanRenderPassBuilder() {
	subPass.setPDepthStencilAttachment(nullptr);
}

VulkanRenderPassBuilder::~VulkanRenderPassBuilder() {

}

VulkanRenderPassBuilder& VulkanRenderPassBuilder::WithColourAttachment(VulkanTexture* texture, bool clear, vk::ImageLayout startLayout, vk::ImageLayout useLayout,  vk::ImageLayout endLayout) {
	allDescriptions.emplace_back(
		vk::AttachmentDescription()
		.setInitialLayout(startLayout)
		.setFinalLayout(endLayout)
		.setFormat(texture->GetFormat())
		.setLoadOp(clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad)
	);
	allReferences.emplace_back(vk::AttachmentReference((uint32_t)allReferences.size(), useLayout));

	return *this;
}

VulkanRenderPassBuilder& VulkanRenderPassBuilder::WithDepthAttachment(VulkanTexture* texture, bool clear, vk::ImageLayout startLayout, vk::ImageLayout useLayout, vk::ImageLayout endLayout) {
	allDescriptions.emplace_back(
		vk::AttachmentDescription()
		.setInitialLayout(startLayout)
		.setFinalLayout(endLayout)
		.setFormat(texture->GetFormat())
		.setLoadOp(clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad)
	);
	depthReference			= vk::AttachmentReference((uint32_t)allReferences.size(), useLayout);
	subPass.setPDepthStencilAttachment(&depthReference);
	return *this;
}

VulkanRenderPassBuilder& VulkanRenderPassBuilder::WithDepthStencilAttachment(VulkanTexture* texture, bool clear, vk::ImageLayout startLayout, vk::ImageLayout useLayout, vk::ImageLayout endLayout) {
	return WithDepthAttachment(texture, clear, startLayout, useLayout, endLayout); //we just get different default parameters!
}

VulkanRenderPassBuilder& VulkanRenderPassBuilder::WithDebugName(const string& name) {
	debugName = name;
	return *this;
}

vk::RenderPass	VulkanRenderPassBuilder::Build(VulkanRenderer& renderer) {
	vk::RenderPass pass;

	subPass.setColorAttachmentCount((uint32_t)allReferences.size())
		.setPColorAttachments(allReferences.data())
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

	vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
		.setAttachmentCount((uint32_t)allDescriptions.size())
		.setPAttachments(allDescriptions.data())
		.setSubpassCount(1)
		.setPSubpasses(&subPass);

	pass = renderer.GetDevice().createRenderPass(renderPassInfo);

	if (!debugName.empty()) {
		renderer.SetDebugName(vk::ObjectType::eRenderPass, (uint64_t)(VkRenderPass)pass, debugName);
	}

	return pass;
}