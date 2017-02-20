#include "standardFirst.h"

#include "system/baseApp.h"
#include "system/sound/cwavefile.h"
#include "system/sound/soundSystem.h"
#include "system/sound/sample.h"

#include <uuids.h>
#include <dshow.h>

//! distance is in cm
float SoundSystem::DISTANCE_FACTOR=0.01f;

SoundSystem* SoundSystem::singleton = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////

SoundSystem::SoundSystem(void)
	: lpDS(NULL)
	, lpDSListener(NULL)
	, graphBuilder(NULL)
	, mediaControl(NULL)
	, mediaEvents(NULL)
{
	soundTrackFilename[0] = 0;
	position=D3DXVECTOR3(0,0,0);
	direction=D3DXVECTOR3(0,0,1);
	masterVolume = 1;
	stInitialised = false;
}

SoundSystem::~SoundSystem(void)
{
	// delete all samples
	std::map<int, Sample*>::iterator pos = sampleBank.begin();
	while (pos != sampleBank.end())
	{
		safe_delete(pos->second);
		pos++;
	}

	safe_release(lpDSListener);
	safe_release(lpDS);

	// release st system
	safe_release(mediaControl);
	safe_release(mediaEvents);
	safe_release(graphBuilder);
}

SoundSystem* SoundSystem::Get()
{
	if (singleton == NULL)
	{
		singleton = new SoundSystem();
	}
	return singleton;
}

void SoundSystem::Destroy()
{
	safe_delete(singleton);
}

LPDIRECTSOUND8 SoundSystem::GetDirectSound(void)
{
	return lpDS;
}

float SoundSystem::GetMasterVolume(void) const
{
	return masterVolume;
}

void SoundSystem::SetMasterVolume(float _masterVolume)
{
	masterVolume=_masterVolume;
}

bool SoundSystem::SetListenerPosition(const D3DXVECTOR3& pos,const D3DXVECTOR3& dirn,bool force)
{
	if (pos!=position || dirn!=direction || force)
	{
		position=pos;
		direction=dirn;

		PreCond(lpDSListener!=NULL);

		DS3DLISTENER params;
		params.dwSize=sizeof(DS3DLISTENER);
		if (SUCCEEDED(lpDSListener->GetAllParameters(&params)))
		{
			params.vPosition=position;

			params.vOrientFront=direction;
			params.vOrientTop=D3DXVECTOR3(0,1,0);
			params.flDistanceFactor=DISTANCE_FACTOR;

			return (SUCCEEDED(lpDSListener->SetAllParameters(&params,DS3D_IMMEDIATE))==TRUE);
		}
		return false;
	}
	return true;
}

bool SoundSystem::Initialise(HWND hwnd)
{
	HRESULT hr;
    DSBUFFERDESC dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary=NULL;

	stInitialised = false;
	if (graphBuilder == NULL)
	{
		CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&graphBuilder);
		if (graphBuilder != NULL)
		{
			if (SUCCEEDED(graphBuilder->QueryInterface(IID_IMediaControl,(void**)&mediaControl)) &&
				SUCCEEDED(graphBuilder->QueryInterface(IID_IMediaEventEx,(void**)&mediaEvents)) &&
				SUCCEEDED(graphBuilder->QueryInterface(IID_IMediaSeeking,(void**)&mediaSeeking)))
			{
				stInitialised = true;
			}
		}
	}

	//! 1. create direct sound object
	hr=DirectSoundCreate8(&DSDEVID_DefaultPlayback,&lpDS,NULL);
	if (hr!=DS_OK)
	{
		Log::GetLog() << "error DirectSoundCreate8 " << hr << System::CR;
		lpDS=NULL;
		return false;
	}
	PreCond(SUCCEEDED(hr=lpDS->SetCooperativeLevel(hwnd,DSSCL_PRIORITY)));

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory(&dsbdesc,sizeof(DSBUFFERDESC));
    dsbdesc.dwSize=sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags=DSBCAPS_CTRL3D|DSBCAPS_PRIMARYBUFFER;
    if (FAILED(hr=lpDS->CreateSoundBuffer(&dsbdesc,&pDSBPrimary,NULL)))
	{
		Log::GetLog() << "error CreateSoundBuffer " << hr << System::CR;
        return false;
	}
	if (FAILED(hr=pDSBPrimary->QueryInterface(IID_IDirectSound3DListener,(VOID**)&lpDSListener)))
    {
		Log::GetLog() << "error get IID_IDirectSound3DListener " << hr << System::CR;
		if (pDSBPrimary!=NULL)
		{
			pDSBPrimary->Release();
		}
		pDSBPrimary = NULL;
        return false;
    }

	safe_release(pDSBPrimary);

	//! set initial listener position
	return SetListenerPosition(position,direction,true);
}

//
// Load a sample and store it in a hashtable under a specified name for accessing later
//
void SoundSystem::LoadStoreSample(int sampleReference, const std::string& filename)
{
	Sample* sample = LoadSample(filename);
	sampleBank[sampleReference] = sample;
}

//
// Load and create a new sample
//
Sample* SoundSystem::LoadSample(const std::string& filename)
{
	HRESULT hr;
	CWaveFile wave;

	PreCond(lpDS != NULL);
	std::string path = System::GetSoundsDirectory() + filename;

	Sample* sample = new Sample(filename);

	hr = wave.Open(path.c_str(), &sample->format, CWaveFile::WAVEFILE_READ);
	if (FAILED(hr))
	{
		delete sample;
		throw new Exception("error loading wave file " + sample->filename);
		return NULL;
	}

	sample->size = wave.GetSize();
	if (sample->size == 0)
	{
		delete sample;
		throw new Exception("error - wavefile is empty "+sample->filename);
		return NULL;
	}

	sample->format = *wave.GetFormat();

	if (sample->format.wFormatTag != WAVE_FORMAT_PCM)
	{
		delete sample;
		throw new Exception(sample->filename+" is not a PCM wave");
		return NULL;
	}

	// calculates what it takes for one ms of sound in terms of bytes
	// multiply this by DSBSIZE_FX_MIN to get a seconds worth of buffer size
	size_t minBufferSize=89;	// this is based on (44.1KHz * 2) / 1000

	DSBUFFERDESC desc;
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_CTRL3D|DSBCAPS_CTRLFX|DSBCAPS_CTRLVOLUME;
	desc.dwBufferBytes = max(sample->size,DSBSIZE_FX_MIN*minBufferSize);
	desc.dwReserved = 0;
	desc.lpwfxFormat = &sample->format;
	desc.guid3DAlgorithm = GUID_NULL; //DS3DALG_HRTF_FULL;

	//! create 3d buffer
	if (FAILED(hr = lpDS->CreateSoundBuffer(&desc, (LPDIRECTSOUNDBUFFER*)&sample->buffer, NULL)))
	{
		delete sample;
		throw new Exception(sample->filename + " sound buffer creation failed");
		return NULL;
	}

	//! set volume to master volume on the buffer
	sample->buffer->SetVolume(long(float(DSBVOLUME_MIN)+float(DSBVOLUME_MAX-DSBVOLUME_MIN) * masterVolume));

	//! lock it and set the data
	LPVOID data1,data2;
	DWORD  size1,size2;
	if (FAILED(hr = sample->buffer->Lock(0, sample->size, &data1, &size1, &data2, &size2, DSBLOCK_ENTIREBUFFER)))
	{
		delete sample;
		throw new Exception("sound buffer lock failed on "+sample->filename);
		return NULL;
	}

	// Reset the wave file to the beginning 
    wave.ResetFile();

	DWORD actualRead;
    if (FAILED(hr = wave.Read((BYTE*)data1,sample->size,&actualRead)))
	{
		delete sample;
		throw new Exception(sample->filename+" error reading wave");
		return NULL;
	}

	if (actualRead==0)
	{
		delete sample;
		throw new Exception(sample->filename+" error reading wave");
		return NULL;
	}

	if (FAILED(hr=sample->buffer->Unlock(data1,size1,data2,size2)))
	{
		delete sample;
		throw new Exception("sound buffer unlock failed on "+sample->filename);
		return NULL;
	}

	Log::GetLog() << "successfully loaded sample " << filename << System::CR;

	return sample;
}

//
// Play a sample
//
bool SoundSystem::PlaySample(Sample* sample, bool looping, bool checkIsPlaying)
{
	HRESULT hr;

	// check if we're already playing - and return true if that is the case
	// (if so requested)
	if (checkIsPlaying && IsSamplePlaying(sample))
	{
		return true;
	}

	if (sample == NULL || sample->buffer == NULL)
	{
		throw new Exception("sample not loaded");
		return false;
	}

	if (FAILED(hr = sample->buffer->SetCurrentPosition(0)))
	{
		return false;
	}

	if (SUCCEEDED(hr = sample->buffer->Play(0, 0, looping ? DSBPLAY_LOOPING : 0)))
	{
		return true;
	}
	return false;
}

bool SoundSystem::PlayStoreSample(int sampleReference, bool looping, bool checkIsPlaying)
{
	Sample* sample = sampleBank[sampleReference];
	if (sample != NULL)
	{
		return PlaySample(sample, looping, checkIsPlaying);
	}
	return false;
}

bool SoundSystem::IsSamplePlaying(Sample* sample)
{
	if (sample == NULL || sample->buffer == NULL)
	{
		throw new Exception("sample is null");
	}
	DWORD status;
	if (SUCCEEDED(sample->buffer->GetStatus(&status)))
	{
		return (status == DSBSTATUS_PLAYING || status == DSBSTATUS_LOOPING);
	}
	return false;
}

bool SoundSystem::IsStoreSamplePlaying(int sampleReference)
{
	Sample* sample = sampleBank[sampleReference];
	if (sample != NULL)
	{
		return IsSamplePlaying(sample);
	}
	return false;
}

bool SoundSystem::ST_Play(const std::string& filename)
{
	if (stInitialised)
	{
		std::string path = System::GetDataDirectory() + filename;
		// render the requested file
		MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, soundTrackFilename, MAX_PATH);
		if (SUCCEEDED(graphBuilder->RenderFile(soundTrackFilename, NULL)))
		{
			if (BaseApp::Get()->PlayMusic())
			{
				mediaControl->Run();
			}
			return true;
		}
	}
	return false;
}

void SoundSystem::ST_Play()
{
	if (stInitialised)
	{
		mediaControl->Stop();
		LONGLONG start = 0;
		mediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
		mediaControl->Run();
	}
}

void SoundSystem::ST_Stop()
{
	if (stInitialised)
	{
		mediaControl->Stop();
	}
}

void SoundSystem::ST_Pause()
{
	if (stInitialised)
	{
		mediaControl->Pause();
	}
}

bool SoundSystem::ST_IsPlaying()
{
	if (stInitialised)
	{
		OAFilterState fs;
		if (SUCCEEDED(mediaControl->GetState(0, &fs)))
		{
			return (fs == State_Running);
		}
	}
	return false;
}

