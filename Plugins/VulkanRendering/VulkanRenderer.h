/******************************************************************************
Class:VulkanRenderer
Implements:RendererBase
Author:Rich Davison
Description:TODO

-_-_-_-_-_-_-_,------,
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../Common/RendererBase.h"

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h> //vulkan hpp needs this included beforehand!
#include <minwindef.h>
#endif

#include <vulkan/vulkan.hpp> //

#include "VulkanShader.h"
#include "VulkanMesh.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"

#include <vector>
#include <string>

using std::string;

namespace NCL {
	namespace Rendering {
		struct UniformData {
			vk::Buffer					buffer;
			vk::MemoryAllocateInfo		allocInfo;
			vk::DeviceMemory			deviceMem;
			vk::DescriptorBufferInfo	descriptorInfo;
		};

		struct VulkanPipeline {
			vk::Pipeline		pipeline;
			vk::PipelineLayout	layout;
		};

		class VulkanRenderer : public RendererBase {
			friend class VulkanMesh;
			friend class VulkanTexture;
			friend class VulkanPipelineBuilder;
			friend class VulkanShaderBuilder;
			friend class VulkanDescriptorSetLayoutBuilder;
			friend class VulkanRenderPassBuilder;
		public:
			VulkanRenderer(Window& window);
			~VulkanRenderer();

		protected:
			void OnWindowResize(int w, int h)	override;
			void BeginFrame()		override;
			void EndFrame()			override;
			void SwapBuffers()		override;

			void SubmitDrawCall(VulkanMesh* m, vk::CommandBuffer& to);

			void SetDebugName(vk::ObjectType t, uint64_t handle, const string& debugName);

			virtual void	CompleteResize();

			vk::DescriptorSet	BuildDescriptorSet(vk::DescriptorSetLayout& layout);

			bool	InitInstance();
			bool	InitGPUDevice();

			int		InitBufferChain();

			bool	CreateDefaultFrameBuffers();

			bool	InitSurface();

			bool	InitDeviceQueue();

			void	ImageTransitionBarrier(vk::CommandBuffer* buffer, vk::Image i, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspect, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, int mipLevel = 0, int layer = 0 );
			void	ImageTransitionBarrier(vk::CommandBuffer* buffer, VulkanTexture* t, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspect, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, int mipLevel = 0, int layer = 0);

			virtual void	InitDefaultRenderPass();
			virtual void	InitDefaultDescriptorPool();

			vk::CommandBuffer BeginCmdBuffer();
			void		EndCmdBufferWait(vk::CommandBuffer&buffer);
			vk::Fence 	EndCmdBuffer(vk::CommandBuffer& buffer);
			void		DestroyCmdBuffer(vk::CommandBuffer& buffer);

			void	BeginSetupCmdBuffer();
			void	EndSetupCmdBuffer();

			void	InitCommandPool();

			void	PresentScreenImage();

			void InitUniformBuffer(UniformData& uniform, void* data, int dataSize);
			void UpdateUniformBuffer(UniformData& uniform, void* data, int dataSize);

			void	UpdateImageDescriptor(vk::DescriptorSet& set, VulkanTexture* t, vk::Sampler sampler, int bindingNum = 0);
			void	UpdateImageDescriptor(vk::DescriptorSet& set, VulkanTexture* t, vk::Sampler sampler, vk::ImageView, vk::ImageLayout forceLayout, int bindingNum = 0);

			vk::Device GetDevice() const {
				return device;
			}

			bool	MemoryTypeFromPhysicalDeviceProps(vk::MemoryPropertyFlags requirements, uint32_t type, uint32_t& index);

			struct SwapChain {
				vk::Image			image;
				vk::ImageView		view;
			};

			vk::DispatchLoaderDynamic* dispatcher;

			vk::SurfaceKHR		surface;
			vk::Format			surfaceFormat;
			vk::ColorSpaceKHR	surfaceSpace;

			vk::Framebuffer*	frameBuffers;

			uint32_t			numFrameBuffers;
			VulkanTexture*		depthBuffer;

			vk::SwapchainKHR		swapChain;

			std::vector<SwapChain*> swapChainList;
			uint32_t				currentSwap;

			vk::Instance		instance;	//API Instance
			vk::PhysicalDevice	gpu;		//GPU in use
			vk::Device			device;		//Device handle

			vk::PhysicalDeviceProperties		deviceProperties;
			vk::PhysicalDeviceMemoryProperties	deviceMemoryProperties;

			vk::PipelineCache	pipelineCache;
			vk::DescriptorPool	defaultDescriptorPool;	//descriptor sets come from here!
			vk::CommandPool		commandPool;			//Source Command Buffers from here

			vk::CommandBuffer	setupCmdBuffer;
			vk::CommandBuffer	frameCmdBuffer;

			vk::RenderPass			defaultRenderPass;
			vk::RenderPassBeginInfo defaultBeginInfo;

			vk::ClearValue	defaultClearValues[2];
			vk::Viewport	defaultViewport;
			vk::Rect2D		defaultScissor;

			vk::Queue			deviceQueue;
			uint32_t			gfxQueueIndex;
			uint32_t			gfxPresentIndex;
		};
	}
}