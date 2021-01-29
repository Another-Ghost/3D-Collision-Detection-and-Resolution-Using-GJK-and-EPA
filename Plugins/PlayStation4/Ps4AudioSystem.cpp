#ifdef _ORBIS
#include "Ps4AudioSystem.h"
#include <libsysmodule.h>

using namespace NCL::PS4;
Ps4AudioSystem::Ps4AudioSystem(int numSources)	{
	sceSysmoduleLoadModule(SCE_SYSMODULE_AUDIO_3D);

	threadFinished = false;

	sceAudio3dInitialize(0);

	SceAudio3dOpenParameters sParameters;
	sceAudio3dGetDefaultOpenParameters(&sParameters);
	sParameters.uiGranularity = 1024;
	sParameters.uiMaxObjects  = numSources;
	sParameters.uiQueueDepth  = 1;

	SceUserServiceUserId userId;
	int ret = sceUserServiceGetInitialUser(&userId);

	ret = sceAudio3dPortOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, &sParameters, &audioPort);

	for (int i = 0; i < numSources; ++i) {
		SceAudio3dPortId id;
		ret = sceAudio3dObjectReserve(audioPort, &id);

		PS4AudioSource* source = new PS4AudioSource(id);

		audioSources.push_back(source);
	}

	testSound = new Sound();
	testSound->LoadFromWAV("/app0/Assets/Audio/lol.wav");

	testEmitter = new Ps4AudioEmitter(testSound, 1.0f);

	testEmitter->SetSound(testSound);

	//audioThread = std::thread(lolfunction);

	threadFinished  = false;
	audioThread = std::thread(&Ps4AudioSystem::AudioThread, this);
}

Ps4AudioSystem::~Ps4AudioSystem()
{
	threadFinished = true;
	audioThread.join();

	for (int i = 0; i < audioSources.size(); ++i) {
		delete audioSources[i];
	}
}

void Ps4AudioSystem::AudioThread()	{
	while (!threadFinished) {
		UpdateSound();
	}
}

void Ps4AudioSystem::UpdateSound() {
	for (PS4AudioSource* s : audioSources) {
		s->Update();
	}

	//debug testing 
	testEmitter->Update(audioSources[0], audioPort);

	sceAudio3dPortAdvance(audioPort);

	sceAudio3dPortPush(audioPort, SCE_AUDIO3D_BLOCKING_SYNC);
}
#endif