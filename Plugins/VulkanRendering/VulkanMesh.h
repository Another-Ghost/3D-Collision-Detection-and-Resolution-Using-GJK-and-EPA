#pragma once
#include "../../Common/MeshGeometry.h"
#include "VulkanRenderer.h"

namespace NCL {
	namespace Rendering {
		struct VulkanVertexSpecification {	
			vk::PipelineVertexInputStateCreateInfo		vertexInfo;
			vk::VertexInputBindingDescription			binding;
			vector<vk::VertexInputAttributeDescription> attributes;
			int numAttributes;
		};

		class VulkanMesh : public MeshGeometry {
		public:
			friend class VulkanRenderer;
			VulkanMesh();
			VulkanMesh(const std::string& filename);
			~VulkanMesh();

			VulkanVertexSpecification* GetVertexSpecification() {
				return &attributeSpec;
			}

			const vk::Buffer& GetVertexBuffer() const {
				return vertexBuffer;
			}

			const vk::Buffer& GetIndexBuffer() const {
				return indexBuffer;
			}

			void UploadToGPU(RendererBase* renderer) override;

		protected:
			VulkanVertexSpecification attributeSpec;			
			
			vk::DeviceMemory	vertexMemory;		
			vk::Buffer			vertexBuffer;

			vk::DeviceMemory	indexMemory;
			vk::Buffer			indexBuffer;

			vk::Device			sourceDevice;
		};
	}
}

