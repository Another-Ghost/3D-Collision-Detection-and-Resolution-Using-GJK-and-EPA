#pragma once
#ifdef _ORBIS
#include "pad.h"
#include "InputBase.h"

namespace NCL {
	namespace PS4 {
		class PS4Input : public InputBase
		{
		public:
			PS4Input();
			~PS4Input();

			void Poll();

		protected:
			void InitController();

			int32_t padHandle;
			ScePadControllerInformation padInfo;
		};
	}
}
#endif
