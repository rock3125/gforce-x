#pragma once

class Sample;

interface IGraphBuilder;
interface IMediaControl;
interface IMediaEventEx;
interface IMediaSeeking;

///////////////////////////////////////////////////////////////////////////////////////////////

class SoundSystem
{
	SoundSystem(const SoundSystem&);
	const SoundSystem& operator=(const SoundSystem&);
public:
	SoundSystem(void);
	virtual ~SoundSystem(void);

	// access sound system singelton
	static SoundSystem* Get();

	// destoy the instance when its all over
	static void Destroy();

	//! initialise primary sound systems
	bool Initialise(HWND hwnd);

	//! access direct sound
	LPDIRECTSOUND8 GetDirectSound(void);

	//! set a new position and direction for a listener
	bool SetListenerPosition(const D3DXVECTOR3& pos,const D3DXVECTOR3& dirn,bool force=false);

	//! get set master volume
	float GetMasterVolume(void) const;
	void SetMasterVolume(float _masterVolume);

	// load a new sample from file
	Sample* LoadSample(const std::string& fname);
	void LoadStoreSample(int sampleReference, const std::string& filename);

	// play a sample
	bool PlaySample(Sample* sample, bool looping, bool checkIsPlaying);
	bool PlayStoreSample(int sampleReference, bool looping, bool checkIsPlaying);

	// is this sample currently playing?
	bool IsSamplePlaying(Sample* sample);
	bool IsStoreSamplePlaying(int sampleReference);

	// play a soundtrack mp3
	bool ST_Play(const std::string& filename);
	void ST_Play();
	void ST_Stop();
	void ST_Pause();
	bool ST_IsPlaying();

private:
	//! direct sound
	LPDIRECTSOUND8			lpDS;

	//! direct sound listener
	LPDIRECTSOUND3DLISTENER lpDSListener;

	//! position and orientation of the listerner
	D3DXVECTOR3				position;
	D3DXVECTOR3				direction;

	//! fixed distance in units (0.01 is cms)
	static float			DISTANCE_FACTOR;

	//! master volume of system
	float					masterVolume;

	// hashtable with samples vs. name
	std::map<int, Sample*> sampleBank;

	// mp3 playback
	WCHAR soundTrackFilename[MAX_PATH];
	IGraphBuilder*	graphBuilder;
	IMediaControl*	mediaControl;
	IMediaEventEx*	mediaEvents;
	IMediaSeeking*	mediaSeeking;

	bool stInitialised;

	// singleton
	static SoundSystem* singleton;
};

