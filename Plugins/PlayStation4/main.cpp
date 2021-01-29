//#include <stdlib.h>
//unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
//size_t       sceLibcHeapSize = 256 * 1024 * 1024;	/* Set up heap area upper limit as 256 MiB */
//
//
//#include "ExampleRenderer.h"
//#include "PS4Input.h"
//
//#include <iostream>
//#include "Ps4SoundSystem.h"
//
//using namespace NCL;
//using namespace PS4;
//
//int main(void) {
//	ExampleRenderer renderer;
//
//	PS4Input		input		= PS4Input();
//
//	Ps4SoundSystem*	soundSystem = new Ps4SoundSystem(8);
//
//	while (true) {
//		input.Poll();
//		renderer.UpdateScene(0.02);
//		renderer.RenderScene();
//		renderer.SwapBuffers();
//
//		if (input.GetButton(0)) {
//			std::cout << "LOL BUTTON" << std::endl;
//		}
//
//		if (input.GetButton(1)) {
//			return 1;
//		}
//	}
//
//	return 1;
//}