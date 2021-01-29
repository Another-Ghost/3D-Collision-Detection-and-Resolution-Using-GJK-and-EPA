#include "VulkanTexture.h"
#include "VulkanRenderer.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace Rendering;

VulkanRenderer* VulkanTexture::vkRenderer = nullptr;

VulkanTexture::VulkanTexture() {
	width		= 0;
	height		= 0;
	mipCount	= 0;
	layerCount	= 0;
	format	 = vk::Format::eUndefined;
	layout	 = vk::ImageLayout::eUndefined;
}

VulkanTexture::~VulkanTexture() {
	vk::Device sourceDevice = vkRenderer->GetDevice();
	sourceDevice.destroyImageView(defaultView);
	sourceDevice.destroyImage(image);
	sourceDevice.freeMemory(deviceMem);
}

int VulkanTexture::CalculateMipCount(int width, int height) {
	return (int)floor(log2(float(std::min(width, height)))) + 1;
}

 VulkanTexture* VulkanTexture::GenerateTextureFromDataInternal(int width, int height, int channelCount, bool isCube, vector<char*>dataSrcs, std::string debugName) {
	 vk::Format				format = vk::Format::eR8G8B8A8Unorm;
	 vk::ImageAspectFlags	aspect = vk::ImageAspectFlagBits::eColor;
	 vk::ImageLayout		layout = vk::ImageLayout::eShaderReadOnlyOptimal;
	 vk::ImageUsageFlags	usage  = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;

	 int mipCount = CalculateMipCount(width, height);
	 //mipCount = 1;
	 VulkanTexture* outTex = GenerateTextureInternal(width, height, mipCount, true, debugName, format, aspect, usage, layout, vk::PipelineStageFlagBits::eFragmentShader);

	 //tex is currently empty, need to fill it with our data from stbimage!
	 int faceSize = width * height * channelCount;
	 int allocationSize = faceSize * (int)dataSrcs.size();

	 vk::Device device = vkRenderer->GetDevice();

	 vk::Buffer	stagingBuffer = device.createBuffer(
		 vk::BufferCreateInfo({}, allocationSize, vk::BufferUsageFlagBits::eTransferSrc)
	 );
	 vk::MemoryRequirements	stagingReqs = {};
	 vk::MemoryAllocateInfo	stagingInfo = {};
	 vk::DeviceMemory		stagingMemory;

	 device.getBufferMemoryRequirements(stagingBuffer, &stagingReqs);

	 stagingInfo.setAllocationSize(allocationSize);
	 vkRenderer->MemoryTypeFromPhysicalDeviceProps(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingReqs.memoryTypeBits, stagingInfo.memoryTypeIndex);

	 vk::CommandBuffer cmdBuffer = vkRenderer->BeginCmdBuffer();

	 stagingMemory = device.allocateMemory(stagingInfo);
	 device.bindBufferMemory(stagingBuffer, stagingMemory, 0);

	 //our buffer now has memory! Copy some texture date to it...
	 char* gpuPtr = (char*)device.mapMemory(stagingMemory, 0, allocationSize);
	 for (int i = 0; i < dataSrcs.size(); ++i) {
		 memcpy(gpuPtr, dataSrcs[i], faceSize);
		 gpuPtr += faceSize;

		 vkRenderer->ImageTransitionBarrier(&cmdBuffer, outTex, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, outTex->aspectType, vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, 0, i);
	 }
	 device.unmapMemory(stagingMemory);

	 vk::BufferImageCopy copyInfo;
	 copyInfo.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor).setMipLevel(0).setLayerCount((uint32_t)dataSrcs.size());
	 copyInfo.imageExtent = vk::Extent3D(width, height, 1);
	 copyInfo.bufferOffset = 0;

	 //Copy from staging buffer to image memory...
	 cmdBuffer.copyBufferToImage(stagingBuffer, outTex->image, vk::ImageLayout::eTransferDstOptimal, copyInfo);

	 if (outTex->mipCount > 1) {
		 outTex->GenerateMipMaps(cmdBuffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::PipelineStageFlagBits::eFragmentShader);
	 }
	 else {
		 vkRenderer->ImageTransitionBarrier(&cmdBuffer, outTex->image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, outTex->aspectType, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader);
		 outTex->defaultView = outTex->GenerateDefaultView(outTex->aspectType);
	 }

	 vkRenderer->EndCmdBufferWait(cmdBuffer);

	 device.destroyBuffer(stagingBuffer); //These can't be destroyed until the cmd buffer has definately completed...
	 device.freeMemory(stagingMemory);

	 return outTex;
}

TextureBase* VulkanTexture::VulkanTextureFromFilename(const std::string& name) {
	char* texData	= nullptr;
	int width		= 0;
	int height		= 0;
	int channels	= 0;
	int flags		= 0;
	TextureLoader::LoadTexture(name, texData, width, height, channels, flags);

	VulkanTexture* cubeTex = GenerateTextureFromDataInternal(width, height, channels, false, { texData }, name);
	delete texData;
	return cubeTex;
};

VulkanTexture* VulkanTexture::VulkanCubemapFromFilename(
	const std::string& negativeXFile, const std::string& positiveXFile,
	const std::string& negativeYFile, const std::string& positiveYFile,
	const std::string& negativeZFile, const std::string& positiveZFile,
	const std::string& debugName) {

	vector<const string*> allFiles = { &negativeXFile, &positiveXFile, &negativeYFile, &positiveYFile, &negativeZFile, &positiveZFile };

	vector<char*> texData(6, nullptr);
	int width[6]	 = { 0 };
	int height[6]	 = { 0 };
	int channels[6]  = { 0 };
	int flags[6]	 = { 0 };

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(*(allFiles[i]), texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return nullptr;
		}
	}

	VulkanTexture* cubeTex = GenerateTextureFromDataInternal(width[0], height[0], channels[0], true, texData, debugName);

	//delete the old texData;
	for (int i = 0; i < 6; ++i) {
		delete texData[i];
	}

	return cubeTex;
}

void	VulkanTexture::InitTextureDeviceMemory(VulkanTexture& img) {
	vk::MemoryRequirements memReqs = vkRenderer->GetDevice().getImageMemoryRequirements(img.image);

	img.allocInfo = vk::MemoryAllocateInfo(memReqs.size);

	bool found = vkRenderer->MemoryTypeFromPhysicalDeviceProps({}, memReqs.memoryTypeBits, img.allocInfo.memoryTypeIndex);

	img.deviceMem = vkRenderer->GetDevice().allocateMemory(img.allocInfo);

	vkRenderer->GetDevice().bindImageMemory(img.image, img.deviceMem, 0);
}

VulkanTexture* VulkanTexture::GenerateTextureInternal(int width, int height, int mipcount, bool isCubemap, std::string debugName, vk::Format format, vk::ImageAspectFlags aspect, vk::ImageUsageFlags usage, vk::ImageLayout outLayout, vk::PipelineStageFlags pipeType) {
	VulkanTexture* tex = new VulkanTexture();
	tex->width		= width;
	tex->height		= height;
	tex->mipCount	= mipcount;
	tex->format		= format;
	tex->aspectType = aspect;
	tex->layerCount = 1;

	tex->createInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setExtent(vk::Extent3D(width, height, 1))
		.setFormat(tex->format)
		.setUsage(usage)
		.setMipLevels(tex->mipCount)
		.setArrayLayers(1)
		.setImageType(vk::ImageType::e2D);

	if (isCubemap) {
		tex->createInfo.setArrayLayers(6).setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
		tex->layerCount = 6;
	}

	tex->image = vkRenderer->GetDevice().createImage(tex->createInfo);

	InitTextureDeviceMemory(*tex);

	tex->defaultView = tex->GenerateDefaultView(tex->aspectType);

	vkRenderer->SetDebugName(vk::ObjectType::eImage, (uint64_t)tex->image.operator VkImage(), debugName);
	vkRenderer->SetDebugName(vk::ObjectType::eImageView, (uint64_t)tex->defaultView.operator VkImageView(), debugName);

	tex->layout = outLayout; //not strictly true until queue submit
	vk::CommandBuffer tempBuffer = vkRenderer->BeginCmdBuffer();
	vkRenderer->ImageTransitionBarrier(&tempBuffer, tex, vk::ImageLayout::eUndefined, outLayout, tex->aspectType, vk::PipelineStageFlagBits::eTopOfPipe, pipeType);
	vkRenderer->EndCmdBufferWait(tempBuffer);
	return tex;
}

VulkanTexture* VulkanTexture::GenerateDepthTexture(int width, int height, string debugName, bool hasStencil, bool useMips) {
	vk::Format			 format		= hasStencil ? vk::Format::eD24UnormS8Uint : vk::Format::eD32Sfloat;
	vk::ImageAspectFlags aspect		= hasStencil ? vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil : vk::ImageAspectFlagBits::eDepth;
	vk::ImageLayout		 layout		= hasStencil ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eDepthAttachmentOptimal;	
	vk::ImageUsageFlags  usage		= vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
	return GenerateTextureInternal(width, height, 1, false, debugName, format, aspect, usage, layout, vk::PipelineStageFlagBits::eEarlyFragmentTests);
}

VulkanTexture* VulkanTexture::GenerateColourTexture(int width, int height, string debugName, bool isFloat, bool useMips) {
	vk::Format			 format = isFloat ? vk::Format::eR32G32B32A32Sfloat : vk::Format::eB8G8R8A8Unorm;
	vk::ImageAspectFlags aspect  = vk::ImageAspectFlagBits::eColor;
	vk::ImageLayout		 layout = vk::ImageLayout::eColorAttachmentOptimal;	
	vk::ImageUsageFlags  usage  = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
	return GenerateTextureInternal(width, height, 1, false, debugName, format, aspect, usage, layout, vk::PipelineStageFlagBits::eColorAttachmentOutput);
}

vk::ImageView  VulkanTexture::GenerateDefaultView(vk::ImageAspectFlags type) {
	vk::ImageViewCreateInfo createInfo =  vk::ImageViewCreateInfo()
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(format)
		.setSubresourceRange(vk::ImageSubresourceRange(type, 0, mipCount, 0, layerCount))
		.setImage(image);
	return vkRenderer->GetDevice().createImageView(createInfo);
}

void VulkanTexture::GenerateMipMaps(vk::CommandBuffer& buffer, vk::ImageLayout endLayout, vk::PipelineStageFlags endFlags) {
	bool localCmdBuffer = false;
	if (!buffer) {
		buffer = vkRenderer->BeginCmdBuffer();
		localCmdBuffer = true;
	}

	for (int layer = 0; layer < layerCount; ++layer) {	
		vkRenderer->ImageTransitionBarrier(&buffer, this, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferSrcOptimal, aspectType, vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eTransfer, 0, layer);
		for (int mip = 1; mip < mipCount; ++mip) {

			vk::ImageBlit blitData;
			blitData.srcSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setMipLevel(mip - 1)
			.setBaseArrayLayer(layer)
			.setLayerCount(1);
			blitData.srcOffsets[0] = vk::Offset3D(0, 0, 0);
			blitData.srcOffsets[1].x = std::max(width >> (mip - 1), 1);
			blitData.srcOffsets[1].y = std::max(height >> (mip - 1), 1);
			blitData.srcOffsets[1].z = 1;

			blitData.dstSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setMipLevel(mip)
			.setLayerCount(1)
			.setBaseArrayLayer(layer);
			blitData.dstOffsets[0] = vk::Offset3D(0, 0, 0);
			blitData.dstOffsets[1].x = std::max(width >> mip, 1);
			blitData.dstOffsets[1].y = std::max(height >> mip, 1);
			blitData.dstOffsets[1].z = 1;

			vkRenderer->ImageTransitionBarrier(&buffer, this, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, aspectType, vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, mip, layer);
			buffer.blitImage(this->image, vk::ImageLayout::eTransferSrcOptimal, this->image, vk::ImageLayout::eTransferDstOptimal, blitData, vk::Filter::eLinear);
			vkRenderer->ImageTransitionBarrier(&buffer, this->image, vk::ImageLayout::eTransferSrcOptimal, endLayout, aspectType, vk::PipelineStageFlagBits::eTransfer, endFlags, mip - 1, layer);

			if (mip < this->mipCount - 1) {
				vkRenderer->ImageTransitionBarrier(&buffer, this->image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, aspectType, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, mip, layer);
			}
			else {
				vkRenderer->ImageTransitionBarrier(&buffer, this->image, vk::ImageLayout::eTransferDstOptimal, endLayout, aspectType, vk::PipelineStageFlagBits::eTransfer, endFlags, mip, layer);
			}
		}
	}
	if (localCmdBuffer) {
		vkRenderer->EndCmdBufferWait(buffer);
	}

	if (defaultView) {
		vkRenderer->GetDevice().destroyImageView(defaultView);
	}

	defaultView = GenerateDefaultView(aspectType);

	layout = endLayout; //Not really true until the below barrier has completed...
}