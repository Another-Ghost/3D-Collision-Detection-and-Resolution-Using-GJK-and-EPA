#pragma once
#ifdef _ORBIS
class PS4Sound
{
public:
	PS4Sound();
	~PS4Sound();
};

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

#ifdef WIN32
typedef unsigned long DWORD;
#else
typedef unsigned int DWORD;
#endif

struct FMTCHUNK {
	short format;
	short channels;
	DWORD srate;
	DWORD bps;
	short balign;
	short samp;
};

class Sound {
public:
	const char*		GetData()		const	{ return data; }
	int				GetBitRate()	const	{ return bitRate; }
	float			GetFrequency()	const	{ return freqRate; }
	int				GetChannels()	const	{ return channels; }
	int				GetSize()		const	{ return size; }
	bool			IsStreaming()	const	{ return streaming; }		//Part 2
	

	double			GetLength();

	static void		AddSound(string n);
	static Sound*	GetSound(string name);

	static void		DeleteSounds();

//protected:
	Sound();
	virtual ~Sound(void);

	void			LoadFromWAV(string filename);
	void			LoadWAVChunkInfo(ifstream &file, string &name, unsigned int &size);

	char*			data;
	//ALuint			buffer;

	bool			streaming;			//Part 2

	float			freqRate;
	double			length;
	unsigned int	bitRate;
	unsigned int	size;
	unsigned int	channels;

	static map<string, Sound*> sounds;
};
#endif