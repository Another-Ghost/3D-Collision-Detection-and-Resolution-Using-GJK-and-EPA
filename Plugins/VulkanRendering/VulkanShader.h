#pragma once

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h> //vulkan hpp needs this included beforehand!
#include <minwindef.h>
#endif

#include "Vulkan/vulkan.hpp"

#include "../../Common/ShaderBase.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace NCL {
	namespace Rendering {
		class VulkanShader : public ShaderBase {
		public:
			friend class VulkanRenderer;
			friend class VulkanShaderBuilder;

			void ReloadShader() override;

			void	FillShaderStageCreateInfo(vk::GraphicsPipelineCreateInfo& info) const;
			~VulkanShader();

		protected:
			void AddBinaryShaderModule(const string& fromFile, ShaderStages stage);
			vk::ShaderModule GetShaderModule(ShaderStages stage) const;

			void SetSourceDevice(vk::Device d);

			void Init();

		protected:			
			VulkanShader();

			static bool		CreateShaderModule(char* data, size_t size, vk::ShaderModule& into, vk::Device& device);

			vk::ShaderModule shaderModules[(int)ShaderStages::SHADER_MAX];

			int stageCount;
			vk::PipelineShaderStageCreateInfo* infos;
			vk::Device sourceDevice;
		};
	}
}

