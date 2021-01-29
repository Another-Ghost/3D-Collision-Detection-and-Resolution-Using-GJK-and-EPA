#ifdef _ORBIS
#include "PS4Shader.h"

#include <iostream>
#include <fstream>
#include <gnmx\shader_parser.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\shader_loader.h>

using std::ifstream;

using namespace NCL;
using namespace NCL::PS4;

PS4Shader::PS4Shader() 
{
	fetchShader		= NULL;
	vertexShader	= NULL;
	pixelShader		= NULL;
}

PS4Shader* PS4Shader::GenerateShader(const string& vertex,const string& pixel) {
	PS4Shader* shader = new PS4Shader();

	shader->GenerateVertexShader(vertex, true);
	shader->GeneratePixelShader(pixel);

	return shader;
}

PS4Shader::~PS4Shader()
{
}

void PS4Shader::GenerateVertexShader(const string&name, bool makeFetch) {
	char*	binData = NULL;	//resulting compiled shader bytes
	int		binSize = 0;
	Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) {
		if (LoadShaderBinary(name, binData, binSize)) {		
			vertexBinary.loadFromMemory(binData, binSize);

			//sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName("LOLBUFFER");

			Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator->allocate(shaderInfo.m_gpuShaderCodeSize, Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator->allocate(shaderInfo.m_vsShader->computeSize(), Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_vsShader, shaderInfo.m_vsShader->computeSize());

			vertexShader = (Gnmx::VsShader*)shaderHeader;
			vertexShader->patchShaderGpuAddress(shaderBinary);
		}
		else {
			std::cout << "Failed to generate vertex shader: " << name << " from binary. " << std::endl;
			return;
		}
	}
	else {
		string shaderString;
		if (LoadShaderText(name, shaderString)) {
			//shaderString now contains the pssl shader data
			//MAGIC GOES HERE
		}
		else {
			std::cout << "Failed to generate vertex shader: " << name << " from raw text. " << std::endl;
			return;
		}
	}

	Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
							shaderInfo.m_gpuShaderCodeSize, name.c_str(), Gnm::kResourceTypeShaderBaseAddress, 0);

	Gnmx::generateInputOffsetsCache(&vertexCache, Gnm::kShaderStageVs, vertexShader);

	if (makeFetch) {
		GenerateFetchShader(binData);
	}
	//delete binData;//done with bindata now!
}

void PS4Shader::GenerateFetchShader(char* binData) {
	fetchShader = garlicAllocator->allocate(Gnmx::Toolkit::calculateMemoryRequiredForVsFetchShader(binData));

	uint32_t shaderModifier;
	Gnmx::generateVsFetchShader(fetchShader, &shaderModifier, vertexShader);

	vertexShader->applyFetchShaderModifier(shaderModifier);
}

void PS4Shader::GeneratePixelShader(const string&name) {
	char*	binData = NULL;	//resulting compiled shader bytes
	int		binSize = 0;
	Gnmx::ShaderInfo shaderInfo;

	if (ShaderIsBinary(name)) {
		if (LoadShaderBinary(name, binData, binSize)) {
			pixelBinary.loadFromMemory(binData, binSize);
			Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator->allocate(shaderInfo.m_gpuShaderCodeSize, Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator->allocate(shaderInfo.m_psShader->computeSize(), Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

			pixelShader = (Gnmx::PsShader*)shaderHeader;
			pixelShader->patchShaderGpuAddress(shaderBinary);
		}
	}
	else {
		bool a = true;
	}

	Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, name.c_str(), Gnm::kResourceTypeShaderBaseAddress, 0);

	Gnmx::generateInputOffsetsCache(&pixelCache, Gnm::kShaderStagePs, pixelShader);

	//delete binData;
}



bool PS4Shader::LoadShaderText(const string &name, string&into) {

	return false;
}

bool PS4Shader::LoadShaderBinary(const string &name, char*& into, int& dataSize) {
	std::ifstream binFile(name, std::ios::binary);

	if (!binFile) {
		return false;
	}
	binFile.seekg(0, ios::end);
	int size = binFile.tellg();

	into = new char[size];

	binFile.seekg(0, ios::beg);
	binFile.read(into, size);

	dataSize = size;

	return true;
}

bool PS4Shader::ShaderIsBinary(const string& name) {
	if (name.length() >= 3 && 
		name[name.length() - 3] == '.' &&
		name[name.length() - 2] == 's' &&
		name[name.length() - 1] == 'b'	) {
		return true;
	}
	return false;
}

void	PS4Shader::SubmitShaderSwitch(Gnmx::GnmxGfxContext& cmdList) {
	cmdList.setActiveShaderStages(Gnm::kActiveShaderStagesVsPs);

	cmdList.setVsShader(vertexShader, 0, fetchShader, &vertexCache);
	cmdList.setPsShader(pixelShader, &pixelCache);
}

int		PS4Shader::GetConstantBuffer(const string &name) {
	sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName(name.c_str());
	if (!constantBuffer) {
		return -1;
	}
	return constantBuffer->m_resourceIndex;
}

#endif