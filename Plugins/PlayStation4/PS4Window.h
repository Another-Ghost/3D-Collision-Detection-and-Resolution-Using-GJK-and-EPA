#pragma once
#ifdef _ORBIS
#include "../../Common/Window.h"

namespace NCL {
	namespace PS4 {
		class PS4Window	: public Window {
		public:
			PS4Window(const std::string& title, int sizeX, int sizeY, bool fullScreen, int offsetX, int offsetY);
			~PS4Window();

			void	LockMouseToWindow(bool lock) override {
			}

			void	ShowOSPointer(bool show) override {
			}

		protected:
			bool InternalUpdate() override;
		};
	}
}
#endif
