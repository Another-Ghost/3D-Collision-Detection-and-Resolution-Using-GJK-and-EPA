#pragma once
#ifdef _ORBIS
#include <audio3d.h>
#include "PS4Sound.h"
#include "PS4AudioSource.h"

namespace NCL {
	namespace PS4 {
		class Ps4AudioEmitter {

		public:
			Ps4AudioEmitter(const Sound* sound, float volume);
			~Ps4AudioEmitter();

			const Sound* GetSound() { return currentSound; }

			void SetSound(const Sound* s);

			void Update(PS4AudioSource*s, SceAudio3dPortId& port);

		protected:
			void SampleFromSound(const Sound*s, int16_t*output, int samplesPerChannel, int startSample);

			SceAudio3dPosition soundPos;

			PS4AudioSource* currentSource;
			const Sound*	currentSound;

			float	gain;
			float	spread;
			double	pitch;
			int		priority;

			int		samplesUsed;
		};
	}
}
#endif

