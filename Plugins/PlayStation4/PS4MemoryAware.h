#pragma once
#ifdef _ORBIS
#include <gnm.h>
#include <gnmx\context.h>

namespace sce{
	namespace Gnmx {
		namespace Toolkit {
			struct IAllocator;
		}
	}
}

namespace NCL {
	namespace PS4 {
		class PS4MemoryAware {
		protected:
			PS4MemoryAware() {}
			~PS4MemoryAware() {}
		protected:
			static sce::Gnmx::Toolkit::IAllocator*	onionAllocator;
			static sce::Gnmx::Toolkit::IAllocator*	garlicAllocator;
			static sce::Gnm::OwnerHandle			ownerHandle;
		};
	}
}
#endif
