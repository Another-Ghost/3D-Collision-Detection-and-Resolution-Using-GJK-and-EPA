#ifdef _ORBIS
#include "PS4MemoryAware.h"
#include <.\graphics\api_gnm\toolkit\allocators.h>
#include <.\graphics\api_gnm\toolkit\stack_allocator.h>
using namespace NCL::PS4;


sce::Gnmx::Toolkit::IAllocator*		PS4MemoryAware::onionAllocator;
sce::Gnmx::Toolkit::IAllocator*		PS4MemoryAware::garlicAllocator;
sce::Gnm::OwnerHandle				PS4MemoryAware::ownerHandle;
#endif