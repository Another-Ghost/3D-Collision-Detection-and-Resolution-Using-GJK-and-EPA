#pragma once
#ifdef _ORBIS
#include "PS4MemoryAware.h"

#include <gnmx.h>

namespace NCL {
	namespace PS4 {
		class PS4Frame :
			public PS4MemoryAware
		{
		public:
			PS4Frame();
			~PS4Frame();

			sce::Gnmx::GnmxGfxContext& GetCommandBuffer() {
				return commandBuffer;
			}

			void BlockUntilReady();
			void StartFrame();
			void EndFrame();

		protected:
			sce::Gnmx::GnmxGfxContext commandBuffer;

			uint64_t* newFrameTag;
		};
	}
}
#endif
