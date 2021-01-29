#ifdef _ORBIS
#include "PS4Frame.h"

#include <gnmx\basegfxcontext.h>
#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>

#include <iostream>

using namespace sce;
using namespace NCL::PS4;

enum FrameState {
	FRAME_READY,
	FRAME_WAITING,
	FRAME_DONE
};

PS4Frame::PS4Frame()
{	
	newFrameTag = (uint64_t*)onionAllocator->allocate(sizeof(uint64_t), 8);

	*newFrameTag = FRAME_READY;

	//Need to allocate memory for our command buffer
	const int bufferBytes = (1 * 1024 * 1024);

	const uint32_t kNumRingEntries = 64;
	const uint32_t cueHeapSize  = Gnmx::ConstantUpdateEngine::computeHeapSize(kNumRingEntries);
	void *constantUpdateEngine  = garlicAllocator->allocate(cueHeapSize, Gnm::kAlignmentOfBufferInBytes);
	void *drawCommandBuffer		= onionAllocator->allocate(bufferBytes , Gnm::kAlignmentOfBufferInBytes);
	void *constantCommandBuffer = onionAllocator->allocate(bufferBytes , Gnm::kAlignmentOfBufferInBytes);

	commandBuffer.init(constantUpdateEngine, kNumRingEntries, drawCommandBuffer, bufferBytes, constantCommandBuffer, bufferBytes);

	Gnm::registerResource(nullptr, ownerHandle, drawCommandBuffer	  , bufferBytes,
		"FrameDrawCommandBuffer", Gnm::kResourceTypeDrawCommandBufferBaseAddress, 0);
	Gnm::registerResource(nullptr, ownerHandle, constantUpdateEngine , bufferBytes,
		"FrameConstantUpdateEngine", Gnm::kResourceTypeDrawCommandBufferBaseAddress, 0);
	Gnm::registerResource(nullptr, ownerHandle, constantCommandBuffer, bufferBytes,
		"FrameConstantCommandBuffer", Gnm::kResourceTypeDrawCommandBufferBaseAddress, 0);
}

PS4Frame::~PS4Frame()
{

}

void PS4Frame::StartFrame() {
	BlockUntilReady();
	*newFrameTag = FRAME_WAITING;

	commandBuffer.reset();
	commandBuffer.initializeDefaultHardwareState();
}

void PS4Frame::BlockUntilReady() {
	int spinCount = 0;

	switch (*newFrameTag)
	{
		case FRAME_READY:	return;
		case FRAME_DONE:	return;
		case FRAME_WAITING: {
			while (*newFrameTag != FRAME_DONE)
			{
				++spinCount;

				if (spinCount > 1000) {
					std::cerr << "Frame block took too long?" << std::endl;
					break;
				}
			}
			if (spinCount > 0) {
				std::cerr << "Frame submission span for " << spinCount << " ticks" << std::endl;
			}
			*newFrameTag = FRAME_READY;
		}break;

		default:
		break;
	}
}

void  PS4Frame::EndFrame() {
	commandBuffer.writeImmediateAtEndOfPipeWithInterrupt(Gnm::kEopFlushCbDbCaches, newFrameTag, FRAME_DONE, Gnm::kCacheActionNone);
}
#endif