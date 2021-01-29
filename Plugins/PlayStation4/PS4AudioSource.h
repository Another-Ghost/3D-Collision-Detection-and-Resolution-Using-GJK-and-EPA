#pragma once
#ifdef _ORBIS
#include <audio3d.h>

namespace NCL {
	namespace PS4 {
		class PS4AudioSource
		{
			friend class Ps4SoundSystem;
		public:


			//protected:	
			PS4AudioSource(SceAudio3dObjectId id);
			~PS4AudioSource();

			void Update();

			SceAudio3dObjectId uiObjectId;
		};
	}
}
#endif
