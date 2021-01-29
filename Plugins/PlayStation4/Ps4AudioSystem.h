#pragma once
#ifdef _ORBIS
#include <thread>
#include <atomic>
#include <vector>

#include <audio3d.h>
#include "PS4AudioSource.h"
#include "Ps4AudioEmitter.h"

namespace NCL {
	namespace PS4 {
		class Ps4AudioSystem
		{
		public:
			Ps4AudioSystem(int audioSources = 32);
			~Ps4AudioSystem();

		protected:
			void AudioThread();
			void UpdateSound();

			std::thread			audioThread;
			std::atomic<bool>	threadFinished;

			//libaudio3D
			SceAudio3dPortId	audioPort;

			std::vector<PS4AudioSource*> audioSources;

			Ps4AudioEmitter*	testEmitter;
			Sound*				testSound;
		};
	}
}
#endif