#pragma once
#include "../../Common/TextureBase.h"
#include <string>
#include <vulkan\vulkan.hpp> //

namespace NCL {
	namespace Rendering {
		class VulkanTexture : public TextureBase
		{
			friend class VulkanRenderer;
		public:

			static VulkanTexture* VulkanCubemapFromFilename(
				const std::string& negativeXFile, const std::string& positiveXFile, 
				const std::string& negativeYFile, const std::string& positiveYFile,
				const std::string& negativeZFile, const std::string& positiveZFile,
				const std::string& debugName = "CubeMap");


			static TextureBase* VulkanTextureFromFilename(const std::string& name);
			static VulkanTexture* GenerateDepthTexture(int width, int height, std::string debugName = "DefaultDepth", bool hasStencil = true, bool mips = false);
			static VulkanTexture* GenerateColourTexture(int width, int height, std::string debugName = "DefaultColour", bool isFloat = false, bool mips = false);

			vk::ImageView GetDefaultView() const {
				return defaultView;
			}

			vk::Format GetFormat() const {
				return format;
			}

			vk::ImageLayout GetLayout() const {
				return layout;
			}

			vk::Image GetImage() const {
				return image;
			}

			~VulkanTexture();

		protected:		
			VulkanTexture();
			void GenerateMipMaps(vk::CommandBuffer& buffer, vk::ImageLayout endLayout, vk::PipelineStageFlags endFlags);

			static void	InitTextureDeviceMemory(VulkanTexture& img);
			static VulkanTexture* GenerateTextureInternal(int width, int height, int mipcount, bool isCube, std::string debugName, vk::Format format, vk::ImageAspectFlags aspect, vk::ImageUsageFlags usage, vk::ImageLayout outLayout, vk::PipelineStageFlags pipeType);

			static VulkanTexture* GenerateTextureFromDataInternal(int width, int height, int channelCount, bool isCube, std::vector<char*>dataSrcs, std::string debugName);


			vk::ImageView  GenerateDefaultView(vk::ImageAspectFlags type);

			static int CalculateMipCount(int width, int height);

			vk::Format				format;
			vk::ImageView			defaultView;
			vk::Image				image;
			vk::DeviceMemory		deviceMem;
			vk::ImageLayout			layout;
	
			vk::MemoryAllocateInfo	allocInfo;
			vk::ImageCreateInfo		createInfo;
			vk::ImageAspectFlags	aspectType;

			int width;
			int height;
			int mipCount;
			int layerCount;

			static void SetRenderer(VulkanRenderer* r) {
				vkRenderer = r;
			}	

			static VulkanRenderer* vkRenderer;
		};
	}
}

