#pragma once
#include "VulkanRenderer.h"

namespace NCL {
namespace Rendering {
	class VulkanRenderPassBuilder
	{
	public:
		VulkanRenderPassBuilder();
		~VulkanRenderPassBuilder();

		VulkanRenderPassBuilder& WithColourAttachment(
			VulkanTexture* texture, 
			bool clear	= true,
			vk::ImageLayout startLayout = vk::ImageLayout::eColorAttachmentOptimal, 
			vk::ImageLayout useLayout	= vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout endLayout	= vk::ImageLayout::eColorAttachmentOptimal
		);

		VulkanRenderPassBuilder& WithDepthAttachment(
			VulkanTexture* texture, 
			bool clear	= true,
			vk::ImageLayout startLayout = vk::ImageLayout::eDepthAttachmentOptimal,
			vk::ImageLayout useLayout	= vk::ImageLayout::eDepthAttachmentOptimal,
			vk::ImageLayout endLayout	= vk::ImageLayout::eDepthAttachmentOptimal
		);

		VulkanRenderPassBuilder& WithDepthStencilAttachment(
			VulkanTexture* texture,
			bool clear = true,
			vk::ImageLayout startLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
			vk::ImageLayout useLayout	= vk::ImageLayout::eDepthStencilAttachmentOptimal,
			vk::ImageLayout endLayout	= vk::ImageLayout::eDepthStencilAttachmentOptimal
		);

		VulkanRenderPassBuilder& WithDebugName(const string& name);

		vk::RenderPass	Build(VulkanRenderer& renderer);

	protected:
		vector<vk::AttachmentDescription>	allDescriptions;
		vector<vk::AttachmentReference>		allReferences;
		vk::AttachmentReference depthReference;
		vk::SubpassDescription	subPass;
		string debugName;
	};
}
}
