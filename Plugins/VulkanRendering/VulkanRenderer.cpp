#include "VulkanRenderer.h"
#include "VulkanMesh.h"
#include "VulkanTexture.h"

#include "../../Common/TextureLoader.h"

#ifdef WIN32
#include "../../Common/Win32Window.h"
using namespace NCL::Win32Code;
#endif

using namespace NCL;
using namespace Rendering;

VulkanRenderer::VulkanRenderer(Window& window) : RendererBase(window) {
	depthBuffer		= nullptr;
	frameBuffers	= nullptr;

	InitInstance();
	InitGPUDevice();
	dispatcher = new vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr, device);

	InitCommandPool();
	InitDefaultDescriptorPool();

	VulkanTexture::SetRenderer(this);
	TextureLoader::RegisterAPILoadFunction(VulkanTexture::VulkanTextureFromFilename);

	OnWindowResize((int)hostWindow.GetScreenSize().x, (int)hostWindow.GetScreenSize().y);

	window.SetRenderer(this);	
	
	pipelineCache = device.createPipelineCache(vk::PipelineCacheCreateInfo());

	vk::Semaphore	presentSempaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
	vk::Fence		fence = device.createFence(vk::FenceCreateInfo());

	currentSwap = device.acquireNextImageKHR(swapChain, UINT64_MAX, presentSempaphore, fence).value;	//Get swap image
}

VulkanRenderer::~VulkanRenderer() {
	delete depthBuffer;

	for (auto& i : swapChainList) {
		device.destroyImageView(i->view);
	};

	for (unsigned int i = 0; i < numFrameBuffers; ++i) {
		device.destroyFramebuffer(frameBuffers[i]);
	}

	device.destroyDescriptorPool(defaultDescriptorPool);
	device.destroySwapchainKHR(swapChain);
	device.destroyCommandPool(commandPool);
	device.destroyRenderPass(defaultRenderPass);
	device.destroyPipelineCache(pipelineCache);
	device.destroy(); //Destroy everything except instance before this gets destroyed!

	delete dispatcher;

	instance.destroySurfaceKHR(surface);
	instance.destroy();

	delete[] frameBuffers;
}

bool VulkanRenderer::InitInstance() {
	vk::ApplicationInfo appInfo = vk::ApplicationInfo(this->hostWindow.GetTitle().c_str());

	appInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);

	const char* usedExtensions[] =	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	const char* usedLayers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	vk::InstanceCreateInfo instanceInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo)
		.setEnabledExtensionCount(sizeof(usedExtensions) / sizeof(char*))
		.setPpEnabledExtensionNames(usedExtensions)
		.setEnabledLayerCount(sizeof(usedLayers) / sizeof(char*))
		.setPpEnabledLayerNames(usedLayers);

	instance = vk::createInstance(instanceInfo);

	return true;
}

bool VulkanRenderer::InitGPUDevice() {
	auto enumResult = instance.enumeratePhysicalDevices();

	if (enumResult.empty()) {
		return false; //Guess there's no Vulkan capable devices?!
	}

	gpu = enumResult[0];
	for (auto& i : enumResult) {
		if (i.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			gpu = i; //Prefer a discrete GPU on multi device machines like laptops
		}
	}

	std::cout << "Vulkan using physical device " << gpu.getProperties().deviceName << std::endl;

	const char* layerNames[]		= { "VK_LAYER_LUNARG_standard_validation" };
	const char* extensionNames[]	= { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	float queuePriority = 0.0f;
	vk::DeviceQueueCreateInfo queueInfo = vk::DeviceQueueCreateInfo()
		.setQueueCount(1)
		.setQueueFamilyIndex(gfxQueueIndex)
		.setPQueuePriorities(&queuePriority);

	vk::PhysicalDeviceFeatures features = vk::PhysicalDeviceFeatures()
		.setMultiDrawIndirect(true)
		.setDrawIndirectFirstInstance(true)
		.setShaderClipDistance(true)
		.setShaderCullDistance(true);

	vk::DeviceCreateInfo createInfo = vk::DeviceCreateInfo()
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&queueInfo)
		.setPEnabledFeatures(&features)
		.setEnabledLayerCount(sizeof(layerNames) / sizeof(char*))
		.setPpEnabledLayerNames(layerNames)
		.setEnabledExtensionCount(sizeof(extensionNames) / sizeof(char*))
		.setPpEnabledExtensionNames(extensionNames);

	InitSurface();
	InitDeviceQueue();

	device		= gpu.createDevice(createInfo);
	deviceQueue = device.getQueue(gfxQueueIndex, 0);
	deviceMemoryProperties = gpu.getMemoryProperties();

	return true;
}

bool VulkanRenderer::InitSurface() {
#ifdef _WIN32
	Win32Window* window = (Win32Window*)&hostWindow;

	vk::Win32SurfaceCreateInfoKHR createInfo;

	createInfo = vk::Win32SurfaceCreateInfoKHR(
		vk::Win32SurfaceCreateFlagsKHR(), window->GetInstance(), window->GetHandle());

	surface = instance.createWin32SurfaceKHR(createInfo);
#endif

	auto formats = gpu.getSurfaceFormatsKHR(surface);

	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
		surfaceFormat	= vk::Format::eB8G8R8A8Unorm;
		surfaceSpace	= formats[0].colorSpace;
	}
	else {
		surfaceFormat	= formats[0].format;
		surfaceSpace	= formats[0].colorSpace;
	}

	return formats.size() > 0;
}

int VulkanRenderer::InitBufferChain() {
	vk::SwapchainKHR oldChain					= swapChain;
	std::vector<SwapChain*> oldSwapChainList	= swapChainList;
	swapChainList.clear();

	vk::SurfaceCapabilitiesKHR surfaceCaps = gpu.getSurfaceCapabilitiesKHR(surface);

	vk::Extent2D swapExtents = vk::Extent2D((int)hostWindow.GetScreenSize().x, (int)hostWindow.GetScreenSize().y);

	auto presentModes = gpu.getSurfacePresentModesKHR(surface); //Type is of vector of PresentModeKHR

	vk::PresentModeKHR idealPresentMode = vk::PresentModeKHR::eFifo;

	for (const auto& i : presentModes) {
		if (i == vk::PresentModeKHR::eMailbox) {
			idealPresentMode = i;
			break;
		}
		else if (i == vk::PresentModeKHR::eImmediate) {
			idealPresentMode = vk::PresentModeKHR::eImmediate; //Might still become mailbox...
		}
	}

	vk::SurfaceTransformFlagBitsKHR idealTransform;

	if (surfaceCaps.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
		idealTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	}
	else {
		idealTransform = surfaceCaps.currentTransform;
	}

	int idealImageCount = surfaceCaps.minImageCount + 1;
	if (surfaceCaps.maxImageCount > 0) {
		idealImageCount = std::min(idealImageCount, (int)surfaceCaps.maxImageCount);
	}

	vk::SwapchainCreateInfoKHR swapInfo;

	swapInfo.setPresentMode(idealPresentMode)
		.setPreTransform(idealTransform)
		.setSurface(surface)
		.setImageColorSpace(surfaceSpace)
		.setImageFormat(surfaceFormat)
		.setImageExtent(swapExtents)
		.setMinImageCount(idealImageCount)
		.setOldSwapchain(oldChain)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

	swapChain = device.createSwapchainKHR(swapInfo);

	if (!oldSwapChainList.empty()) {
		for (unsigned int i = 0; i < numFrameBuffers; ++i) {
			device.destroyImageView(oldSwapChainList[i]->view);
			delete oldSwapChainList[i];
		}
	}
	if (oldChain) {
		device.destroySwapchainKHR(oldChain);
	}

	auto images = device.getSwapchainImagesKHR(swapChain);

	for (auto& i : images) {
		vk::ImageViewCreateInfo viewCreate = vk::ImageViewCreateInfo()
			.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
			.setFormat(surfaceFormat)
			.setImage(i)
			.setViewType(vk::ImageViewType::e2D);

		SwapChain* chain = new SwapChain();

		chain->image = i;

		ImageTransitionBarrier(&setupCmdBuffer, i, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput);

		chain->view = device.createImageView(viewCreate);

		swapChainList.push_back(chain);
	}

	return (int)images.size();
}

void	VulkanRenderer::ImageTransitionBarrier(vk::CommandBuffer* buffer, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspect, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, int mipLevel, int layer) {
	if (!buffer) {
		buffer = &setupCmdBuffer;
	}
	vk::ImageSubresourceRange subRange = vk::ImageSubresourceRange(aspect, mipLevel, 1, layer, 1);

	vk::ImageMemoryBarrier memoryBarrier = vk::ImageMemoryBarrier()
		.setSubresourceRange(subRange)
		.setImage(image)
		.setOldLayout(oldLayout)
		.setNewLayout(newLayout);

	if (newLayout == vk::ImageLayout::eTransferDstOptimal) {
		memoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	}
	else if (newLayout == vk::ImageLayout::eTransferSrcOptimal) {
		memoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
	}
	else if (newLayout == vk::ImageLayout::eColorAttachmentOptimal) {
		memoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
	}
	else if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
		memoryBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	else if (newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		memoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead; //added last bit?!?
	}

	buffer->pipelineBarrier(srcStage, dstStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &memoryBarrier);
}

void	VulkanRenderer::ImageTransitionBarrier(vk::CommandBuffer* buffer, VulkanTexture* t, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspect, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, int mipLevel, int layer) {
	if (!buffer) {
		buffer = &setupCmdBuffer;
	}
	ImageTransitionBarrier(buffer, t->GetImage(), oldLayout, newLayout, aspect, srcStage, dstStage, mipLevel, layer);
	t->layout = newLayout;
}

void	VulkanRenderer::InitCommandPool() {
	commandPool = device.createCommandPool(vk::CommandPoolCreateInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer, gfxQueueIndex));

	auto buffers = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(
		commandPool, vk::CommandBufferLevel::ePrimary, 1));

	frameCmdBuffer = buffers[0];
}

vk::CommandBuffer VulkanRenderer::BeginCmdBuffer() {
	vk::CommandBufferAllocateInfo bufferInfo = vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1);

	auto buffers = device.allocateCommandBuffers(bufferInfo); //Func returns a vector!

	vk::CommandBuffer &newBuf = buffers[0];

	vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();

	newBuf.begin(beginInfo);
	return newBuf;
}

void		VulkanRenderer::EndCmdBufferWait(vk::CommandBuffer& buffer) {
	vk::Fence fence = EndCmdBuffer(buffer);

	if (!fence) {
		return;
	}

	if (device.waitForFences(1, &fence, true, UINT64_MAX) != vk::Result::eSuccess) {
		std::cout << "Device queue submission taking too long?" << std::endl;
	};

	device.freeCommandBuffers(commandPool, buffer);

	device.destroyFence(fence);
}

vk::Fence 	VulkanRenderer::EndCmdBuffer(vk::CommandBuffer& buffer) {
	vk::Fence fence;
	if (buffer) {
		buffer.end();
	}
	else {
		return fence;
	}

	fence = device.createFence(vk::FenceCreateInfo());

	vk::SubmitInfo submitInfo = vk::SubmitInfo();
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&buffer);

	deviceQueue.submit(submitInfo, fence);
	return fence;
}

void		VulkanRenderer::DestroyCmdBuffer(vk::CommandBuffer& buffer) {

}

void VulkanRenderer::BeginSetupCmdBuffer() {
	setupCmdBuffer = BeginCmdBuffer();
}

void	VulkanRenderer::EndSetupCmdBuffer() {
	EndCmdBufferWait(setupCmdBuffer);
}

bool VulkanRenderer::InitDeviceQueue() {
	vector<vk::QueueFamilyProperties> deviceQueueProps = gpu.getQueueFamilyProperties();

	VkBool32 supportsPresent = false;
	gfxQueueIndex	= -1;
	gfxPresentIndex = -1;

	for (unsigned int i = 0; i < deviceQueueProps.size(); ++i) {
		supportsPresent = gpu.getSurfaceSupportKHR(i, surface);

		if (deviceQueueProps[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			gfxQueueIndex = i;
			if (supportsPresent) {
				gfxPresentIndex = i;
				break;
			}
		}
	}

	if (gfxPresentIndex == -1) {
		for (unsigned int i = 0; i < deviceQueueProps.size(); ++i) {
			supportsPresent = gpu.getSurfaceSupportKHR(i, surface);
			if (supportsPresent) {
				gfxPresentIndex = i;
				break;
			}
		}
	}

	if (gfxQueueIndex == -1 || gfxPresentIndex == -1) {
		return false;
	}

	return true;
}

void VulkanRenderer::OnWindowResize(int width, int height) {
	if (width == currentWidth && height == currentHeight) {
		return;
	}
	currentWidth	= width;
	currentHeight	= height;

	defaultViewport = vk::Viewport(0.0f, (float)currentHeight, (float)currentWidth, -(float)currentHeight, 0.0f, 1.0f);
	defaultScissor	= vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(currentWidth, currentHeight));

	defaultClearValues[0] = vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{0.2f, 0.2f, 0.2f, 1.0f}));
	defaultClearValues[1] = vk::ClearValue(vk::ClearDepthStencilValue(1.0f, 0));
	BeginSetupCmdBuffer();
	std::cout << "calling resize! new dimensions: " << currentWidth << " , " << currentHeight << std::endl;
	vkDeviceWaitIdle(device);

	delete depthBuffer;
	depthBuffer = VulkanTexture::GenerateDepthTexture((int)hostWindow.GetScreenSize().x, (int)hostWindow.GetScreenSize().y);
	
	numFrameBuffers = InitBufferChain();

	InitDefaultRenderPass();
	CreateDefaultFrameBuffers();

	vkDeviceWaitIdle(device);

	CompleteResize();

	EndSetupCmdBuffer();
}

void VulkanRenderer::CompleteResize() {

}

void	VulkanRenderer::BeginFrame() {
	vk::CommandBufferInheritanceInfo inheritance;
	vk::CommandBufferBeginInfo bufferBegin = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlags(), &inheritance);
	frameCmdBuffer.begin(bufferBegin);
	frameCmdBuffer.setViewport(0, 1, &defaultViewport);
	frameCmdBuffer.setScissor(0, 1, &defaultScissor);

	//Wait until the swap image is actually available!
	ImageTransitionBarrier(&frameCmdBuffer, swapChainList[currentSwap]->image, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput);
	//frameCmdBuffer.beginRenderPass(defaultBeginInfo, vk::SubpassContents::eInline);
}

void	VulkanRenderer::EndFrame() {
	//frameCmdBuffer.endRenderPass();
	frameCmdBuffer.end();
	vk::SubmitInfo submitInfo = vk::SubmitInfo(0, nullptr, nullptr, 1, &frameCmdBuffer, 0, nullptr);
	vk::Fence fence = device.createFence(vk::FenceCreateInfo());

	vk::Result result = deviceQueue.submit(1, &submitInfo, fence);
	device.waitForFences(fence, true, ~0);
	device.destroyFence(fence);
}

void VulkanRenderer::SwapBuffers() {
	PresentScreenImage();

	deviceQueue.presentKHR(vk::PresentInfoKHR(0, nullptr, 1, &swapChain, &currentSwap, nullptr));

	vk::Semaphore	presentSempaphore = device.createSemaphore(vk::SemaphoreCreateInfo());
	vk::Fence		fence = device.createFence(vk::FenceCreateInfo());

	currentSwap = device.acquireNextImageKHR(swapChain, UINT64_MAX, presentSempaphore, fence).value;	//Get swap image

	defaultBeginInfo = vk::RenderPassBeginInfo()
		.setRenderPass(defaultRenderPass)
		.setFramebuffer(frameBuffers[currentSwap])
		.setRenderArea(defaultScissor)
		.setClearValueCount(sizeof(defaultClearValues) / sizeof(vk::ClearValue))
		.setPClearValues(defaultClearValues);

	device.waitForFences(fence, true, ~0);	
	
	device.destroySemaphore(presentSempaphore);
	device.destroyFence(fence);
}

void	VulkanRenderer::InitDefaultRenderPass() {
	if (defaultRenderPass) {
		device.destroyRenderPass(defaultRenderPass);
	}
	vk::AttachmentDescription attachments[] = {
		vk::AttachmentDescription()
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFormat(surfaceFormat)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
	,
		vk::AttachmentDescription()
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setFormat(depthBuffer->GetFormat())
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
	};

	vk::AttachmentReference references[] = {
		vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal),
		vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal)
	};

	vk::SubpassDescription subPass = vk::SubpassDescription()
		.setColorAttachmentCount(1)
		.setPColorAttachments(&references[0])
		.setPDepthStencilAttachment(&references[1])
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

	vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
		.setAttachmentCount(2)
		.setPAttachments(attachments)
		.setSubpassCount(1)
		.setPSubpasses(&subPass);

	defaultRenderPass = device.createRenderPass(renderPassInfo);
}

void	VulkanRenderer::PresentScreenImage() {
	BeginSetupCmdBuffer();

	vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
		.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
		.setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
		.setImage(swapChainList[currentSwap]->image)
		.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setNewLayout(vk::ImageLayout::ePresentSrcKHR);

	barrier.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

	setupCmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

	EndSetupCmdBuffer();
}

bool VulkanRenderer::CreateDefaultFrameBuffers() {
	if (frameBuffers) {
		for (unsigned int i = 0; i < numFrameBuffers; ++i) {
			device.destroyFramebuffer(frameBuffers[i]);
		}
	}
	else {
		frameBuffers = new vk::Framebuffer[numFrameBuffers];
	}

	vk::ImageView attachments[2];
	
	vk::FramebufferCreateInfo createInfo = vk::FramebufferCreateInfo()
		.setWidth((int)hostWindow.GetScreenSize().x)
		.setHeight((int)hostWindow.GetScreenSize().y)
		.setLayers(1)
		.setAttachmentCount(2)
		.setPAttachments(attachments)
		.setRenderPass(defaultRenderPass);

	for (unsigned int i = 0; i < numFrameBuffers; ++i) {
		attachments[0]	= swapChainList[i]->view;
		attachments[1]	= depthBuffer->defaultView;
		frameBuffers[i] = device.createFramebuffer(createInfo);
	}

	defaultBeginInfo = vk::RenderPassBeginInfo()
		.setRenderPass(defaultRenderPass)
		.setFramebuffer(frameBuffers[currentSwap])
		.setRenderArea(defaultScissor)
		.setClearValueCount(sizeof(defaultClearValues) / sizeof(vk::ClearValue))
		.setPClearValues(defaultClearValues);

	return true;
}

bool	VulkanRenderer::MemoryTypeFromPhysicalDeviceProps(vk::MemoryPropertyFlags requirements, uint32_t type, uint32_t& index) {
	for (int i = 0; i < 32; ++i) {
		if ((type & 1) == 1) {	//We care about this requirement
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & requirements) == requirements) {
				index = i;
				return true;
			}
		}
		type >>= 1; //Check next bit
	}
	return false;
}

void	VulkanRenderer::InitDefaultDescriptorPool() {
	int maxSets = 128; //how many times can we ask the pool for a descriptor set?
	vk::DescriptorPoolSize poolSizes[] = {
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 128),
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 128)
	};

	vk::DescriptorPoolCreateInfo poolCreate;
	poolCreate.setPoolSizeCount(sizeof(poolSizes) / sizeof(vk::DescriptorPoolSize));
	poolCreate.setPPoolSizes(poolSizes);
	poolCreate.setMaxSets(maxSets);

	defaultDescriptorPool = device.createDescriptorPool(poolCreate);
}

void VulkanRenderer::SetDebugName(vk::ObjectType t, uint64_t handle, const string& debugName) {
	device.setDebugUtilsObjectNameEXT(
		vk::DebugUtilsObjectNameInfoEXT()
		.setObjectType(t)
		.setObjectHandle(handle)
		.setPObjectName(debugName.c_str()), *dispatcher
	);
};

void	VulkanRenderer::UpdateImageDescriptor(vk::DescriptorSet& set, VulkanTexture* tex, vk::Sampler sampler, vk::ImageView forceView, vk::ImageLayout forceLayout, int bindingNum) {
	vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo()
		.setSampler(sampler)
		.setImageView(forceView)
		.setImageLayout(forceLayout);

	vk::WriteDescriptorSet descriptorWrite = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setDstSet(set)
		.setDstBinding(bindingNum)
		.setDescriptorCount(1)
		.setPImageInfo(&imageInfo);

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void	VulkanRenderer::UpdateImageDescriptor(vk::DescriptorSet& set, VulkanTexture* tex, vk::Sampler sampler, int bindingNum) {
	if (!tex) {
		std::cout << __FUNCTION__ << " tex parameter is NULL!\n";
		return;
	}

	vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo()
		.setSampler(sampler)
		.setImageView(tex->GetDefaultView())
		.setImageLayout(tex->GetLayout());

	vk::WriteDescriptorSet descriptorWrite = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setDstSet(set)
		.setDstBinding(bindingNum)
		.setDescriptorCount(1)
		.setPImageInfo(&imageInfo);

	device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

void VulkanRenderer::InitUniformBuffer(UniformData& uniform, void* data, int dataSize) {
	uniform.buffer = device.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), dataSize, vk::BufferUsageFlagBits::eUniformBuffer));

	uniform.descriptorInfo.buffer = uniform.buffer;
	uniform.descriptorInfo.range = dataSize;//reqs.size;

	vk::MemoryRequirements reqs = device.getBufferMemoryRequirements(uniform.buffer);
	uniform.allocInfo = vk::MemoryAllocateInfo(reqs.size);

	bool found = MemoryTypeFromPhysicalDeviceProps(vk::MemoryPropertyFlagBits::eHostVisible, reqs.memoryTypeBits, uniform.allocInfo.memoryTypeIndex);

	uniform.deviceMem = device.allocateMemory(uniform.allocInfo);

	device.bindBufferMemory(uniform.buffer, uniform.deviceMem, 0);

	UpdateUniformBuffer(uniform, data, dataSize);
}

void VulkanRenderer::UpdateUniformBuffer(UniformData& uniform, void* data, int dataSize) {
	void* mappedData = device.mapMemory(uniform.deviceMem, 0, uniform.allocInfo.allocationSize);
	memcpy(mappedData, data, dataSize);
	device.unmapMemory(uniform.deviceMem);
}

vk::DescriptorSet	VulkanRenderer::BuildDescriptorSet(vk::DescriptorSetLayout& layout) {
	vk::DescriptorSetAllocateInfo allocateInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(defaultDescriptorPool)
		.setDescriptorSetCount(1)
		.setPSetLayouts(&layout);

	vk::DescriptorSet newSet;
	device.allocateDescriptorSets(&allocateInfo, &newSet);
	return newSet;
}

void VulkanRenderer::SubmitDrawCall(VulkanMesh* m, vk::CommandBuffer& to) {
	VkDeviceSize baseOffset = 0;
	int instanceCount = 1;

	to.bindVertexBuffers(0, 1, &m->GetVertexBuffer(), &baseOffset);

	if (m->GetIndexCount() > 0) {
		to.bindIndexBuffer(m->GetIndexBuffer(), 0, vk::IndexType::eUint32);

		to.drawIndexed(m->GetIndexCount(), instanceCount, 0, 0, 0);
	}
	else {
		to.draw(m->GetVertexCount(), instanceCount, 0, 0);
	}
}
