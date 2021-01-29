#ifdef _ORBIS
#include "Ps4AudioEmitter.h"

using namespace NCL::PS4;
Ps4AudioEmitter::Ps4AudioEmitter(const Sound* sound, float volume)
{
	gain		= volume;
	spread		= 0.0f;
	pitch		= 1.0f;
	priority	= 1;

	currentSound = sound;

	currentSource = NULL;

	samplesUsed = 0;

	soundPos.fX = 0.0f;
	soundPos.fY = 0.0f;
	soundPos.fZ = 0.0f;

//	uiObjectId = SCE_AUDIO3D_OBJECT_INVALID;
}


Ps4AudioEmitter::~Ps4AudioEmitter()
{
	//if (uiObjectId != SCE_AUDIO3D_OBJECT_INVALID) {
	//	sceAudio3dObjectUnreserve
	//}
}

#define SAMPLE_GRANULARITY 1024

void Ps4AudioEmitter::Update(PS4AudioSource*s, SceAudio3dPortId& port) {
	currentSource = s;

	SceAudio3dAttribute sAttributes[5];

	sAttributes[0].uiAttributeId	= SCE_AUDIO3D_ATTRIBUTE_GAIN;
	sAttributes[0].pValue			= &gain;
	sAttributes[0].szValue			= sizeof(float);

	sAttributes[1].uiAttributeId	= SCE_AUDIO3D_ATTRIBUTE_SPREAD;
	sAttributes[1].pValue			= &spread;
	sAttributes[1].szValue			= sizeof(float);

	sAttributes[2].uiAttributeId	= SCE_AUDIO3D_ATTRIBUTE_PRIORITY;
	sAttributes[2].pValue			= &priority;
	sAttributes[2].szValue			= sizeof(int);

	sAttributes[3].uiAttributeId	= SCE_AUDIO3D_ATTRIBUTE_POSITION;
	sAttributes[3].pValue			= &soundPos;
	sAttributes[3].szValue			= sizeof(SceAudio3dPosition);

	__attribute__((aligned(64))) int16_t iSampleBuffer[SAMPLE_GRANULARITY];
	memset(iSampleBuffer, 0, sizeof(iSampleBuffer));

	SceAudio3dPcm sPcm;

	sPcm.eFormat		= SCE_AUDIO3D_FORMAT_S16;
	sPcm.pSampleBuffer	= iSampleBuffer;
	sPcm.uiNumSamples	= SAMPLE_GRANULARITY;

	sAttributes[4].uiAttributeId = SCE_AUDIO3D_ATTRIBUTE_PCM;
	sAttributes[4].pValue		 = &sPcm;
	sAttributes[4].szValue		 = sizeof(sPcm);

	SampleFromSound(currentSound, iSampleBuffer , SAMPLE_GRANULARITY, samplesUsed);
	samplesUsed += SAMPLE_GRANULARITY;

	sceAudio3dObjectSetAttributes(port, s->uiObjectId, 5, sAttributes);
}

void Ps4AudioEmitter::SetSound(const Sound* s) {
	currentSound = s;

}

void Ps4AudioEmitter::SampleFromSound(const Sound*s, int16_t* output, int samplesPerChannel, int startSample) {
	int currentSamplePos = startSample;
	currentSamplePos = (currentSamplePos + 1) % (s->GetSize() / 2);

	int16_t* sData = (int16_t*)s->GetData();

	for (int i = 0; i < samplesPerChannel; ++i) {
		*output = sData[currentSamplePos];
		output++;

		currentSamplePos = (currentSamplePos + 1) % (s->GetSize() / 2);
	}
}
#endif