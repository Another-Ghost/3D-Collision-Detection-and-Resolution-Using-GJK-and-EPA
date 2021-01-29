#include "VulkanMesh.h"

using namespace NCL;
using namespace Rendering;

//These are both carefully arranged to match the MeshBuffer enum class!
vk::Format attributeFormats[] = {
	vk::Format::eR32G32B32Sfloat,	//Positions have this format
	vk::Format::eR32G32B32A32Sfloat,//Colours
	vk::Format::eR32G32Sfloat,		//TexCoords
	vk::Format::eR32G32B32Sfloat,	//Normals
	vk::Format::eR32G32B32Sfloat,	//Tangents
	vk::Format::eR32G32B32A32Sfloat,//Skel indices
	vk::Format::eR32G32B32A32Sfloat,//Skel Weights
};

size_t attributeSizes[] = {
	sizeof(Vector3),
	sizeof(Vector4),
	sizeof(Vector2),
	sizeof(Vector3),
	sizeof(Vector3),
	sizeof(Vector4),
	sizeof(Vector4),
};

VulkanMesh::VulkanMesh()	{

}

VulkanMesh::VulkanMesh(const std::string& filename) : MeshGeometry(filename) {

}

VulkanMesh::~VulkanMesh()	{
	if (indexBuffer) {
		sourceDevice.destroyBuffer(indexBuffer);
		sourceDevice.freeMemory(indexMemory);
	}
	if (vertexBuffer) {
		sourceDevice.destroyBuffer(vertexBuffer);
		sourceDevice.freeMemory(vertexMemory);
	}
}

void VulkanMesh::UploadToGPU(RendererBase* r)  {
	if (!ValidateMeshData()) {
		return;
	}
	VulkanRenderer* renderer = (VulkanRenderer*)r;

	sourceDevice = renderer->GetDevice();

	vector< int >			attributeTypes;
	vector< const void*>	attributePtrs;

	int strideSize		= 0; //how much data each vertex takes up - so how much we much jump over to get to the next vertex in a big buffer

	auto atrributeFunc = [&](VertexAttribute attribute, const void* data) {
		if (data) {
			attributeTypes.emplace_back(attribute);
			attributePtrs.emplace_back(data);
			strideSize += (int)attributeSizes[attribute];
		}
	};

	atrributeFunc(VertexAttribute::Positions,		GetPositionData().data());	
	atrributeFunc(VertexAttribute::Colours,			GetColourData().data());
	atrributeFunc(VertexAttribute::TextureCoords,	GetTextureCoordData().data());
	atrributeFunc(VertexAttribute::Normals,			GetNormalData().data());
	atrributeFunc(VertexAttribute::Tangents,		GetTangentData().data());
	atrributeFunc(VertexAttribute::JointWeights,	GetSkinWeightData().data());
	atrributeFunc(VertexAttribute::JointIndices,	GetSkinIndexData().data());

	attributeSpec.numAttributes = (int)attributeTypes.size();

	vertexBuffer = sourceDevice.createBuffer(
		vk::BufferCreateInfo({}, strideSize * GetVertexCount(), vk::BufferUsageFlagBits::eVertexBuffer)
	);

	vk::MemoryRequirements memReqs = sourceDevice.getBufferMemoryRequirements(vertexBuffer);

	vk::MemoryAllocateInfo memInfo = vk::MemoryAllocateInfo(memReqs.size);
	if (!renderer->MemoryTypeFromPhysicalDeviceProps(
		vk::MemoryPropertyFlagBits::eHostVisible, memReqs.memoryTypeBits, memInfo.memoryTypeIndex)) {
		return; //Oh no - Turns out we don't have the memory requirement for this!
	}

	vertexMemory = sourceDevice.allocateMemory(memInfo);
	sourceDevice.bindBufferMemory(vertexBuffer, vertexMemory, 0);
	//need to now copy vertex data to device memory
	char* dataPtr = (char*)sourceDevice.mapMemory(vertexMemory, 0, memInfo.allocationSize);
	for (unsigned int v = 0; v < GetVertexCount(); ++v) { //for every vertex
		for (int i = 0; i < attributeSpec.numAttributes; ++i) { //copy its next attribute to the GPU
			size_t copySize = attributeSizes[attributeTypes[i]];
			memcpy(dataPtr, (char*)attributePtrs[i] + (v * copySize), copySize);
			dataPtr += copySize;
		}
	}
	sourceDevice.unmapMemory(vertexMemory);

	int currentOffset = 0;
	for (int i = 0; i < attributeSpec.numAttributes; ++i) {
		int realIndex = attributeTypes[i]; //Shaders are locked to specific ids due to the binding locations, this converts to that locked ID

		vk::VertexInputAttributeDescription description;

		description.binding		= 0; //in in the same single bound buffer
		description.format		= attributeFormats[realIndex];
		description.location	= realIndex;
		description.offset		= currentOffset;

		attributeSpec.attributes.emplace_back(description);

		currentOffset += (int)attributeSizes[realIndex];
	}

	attributeSpec.binding = vk::VertexInputBindingDescription(0, strideSize, vk::VertexInputRate::eVertex);

	attributeSpec.vertexInfo = vk::PipelineVertexInputStateCreateInfo({},
		1, & attributeSpec.binding,	//How many buffers this mesh will be taking data from
		attributeSpec.numAttributes, &attributeSpec.attributes[0] //how many attributes + attribute info from above
	);

	//Make the index buffer if there are any!
	if (GetIndexCount() > 0) {
		indexBuffer = sourceDevice.createBuffer(
			vk::BufferCreateInfo({}, sizeof(int) * GetIndexCount(), vk::BufferUsageFlagBits::eIndexBuffer)
		);

		vk::MemoryRequirements memReqs = sourceDevice.getBufferMemoryRequirements(indexBuffer);

		vk::MemoryAllocateInfo memInfo = vk::MemoryAllocateInfo(memReqs.size);
		if (!renderer->MemoryTypeFromPhysicalDeviceProps(
			vk::MemoryPropertyFlagBits::eHostVisible, memReqs.memoryTypeBits, memInfo.memoryTypeIndex)) {
			return; //Oh no - Turns out we don't have the memory requirement for this!
		}

		indexMemory = sourceDevice.allocateMemory(memInfo);
		char* dataPtr = (char*)sourceDevice.mapMemory(indexMemory, 0, memInfo.allocationSize);

		memcpy(dataPtr, GetIndexData().data(), sizeof(int) * GetIndexCount());

		sourceDevice.unmapMemory(indexMemory);

		sourceDevice.bindBufferMemory(indexBuffer, indexMemory, 0);
	}

	if (!debugName.empty()) {
		renderer->SetDebugName(vk::ObjectType::eBuffer, (uint64_t)(VkBuffer)vertexBuffer, debugName + " attributes");
		if (GetIndexCount() > 0) {
			renderer->SetDebugName(vk::ObjectType::eBuffer, (uint64_t)(VkBuffer)indexBuffer, debugName + " indices");
		}
	}
}