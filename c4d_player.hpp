#include "jni.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>
using namespace std;
#define assert(x) if(!(x)) throw runtime_error("Assert Failed: " #x)
static const SLEnvironmentalReverbSettings reverbSettings =
    SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

class AudioPlayer
{
private:
    SLObjectItf engineObject;///
    SLEngineItf engineEngine;
    SLObjectItf outputMixObject;///
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
    SLObjectItf uriPlayerObject;///
    SLPlayItf uriPlayerPlay;///
    SLSeekItf uriPlayerSeek;///
    bool isready;
public:
    AudioPlayer()
    {
        SLresult result;
        // create engine
        result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
        assert(SL_RESULT_SUCCESS == result);
        // realize the engine
        result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);
        // get the engine interface
        result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
        assert(SL_RESULT_SUCCESS == result);
        // create output mix
        const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
        const SLboolean req[1] = {SL_BOOLEAN_FALSE};
        result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
        assert(SL_RESULT_SUCCESS == result);
        // realize the output mix
        result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
        assert(SL_RESULT_SUCCESS == result);
        isready=false;
    }
    int load(char* uri,bool isloop)
    {
        if(isready) return -2;

        SLresult result;
        // config audio source
        SLDataLocator_URI loc_uri = {SL_DATALOCATOR_URI, (SLchar *) uri};
        SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
        SLDataSource audioSrc = {&loc_uri, &format_mime};
        // config audio sink
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
        SLDataSink audioSnk = {&loc_outmix, NULL};
        // create audio player
        const SLInterfaceID ids[1] = {SL_IID_SEEK};
        const SLboolean req[1] = {SL_BOOLEAN_TRUE};
        result = (*engineEngine)->CreateAudioPlayer(engineEngine, &uriPlayerObject, &audioSrc, &audioSnk, 1, ids, req);
        assert(SL_RESULT_SUCCESS == result);
        // realize the player
        result = (*uriPlayerObject)->Realize(uriPlayerObject, SL_BOOLEAN_FALSE);
        if (SL_RESULT_SUCCESS != result)
        {
            (*uriPlayerObject)->Destroy(uriPlayerObject);
            uriPlayerObject = NULL;
            return -1;
        }
        // get the play interface
        result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_PLAY, &uriPlayerPlay);
        assert(SL_RESULT_SUCCESS == result);
        // get the seek interface
        result = (*uriPlayerObject)->GetInterface(uriPlayerObject, SL_IID_SEEK, &uriPlayerSeek);
        assert(SL_RESULT_SUCCESS == result);
        // enable whole file looping
        result = (*uriPlayerSeek)->SetLoop(uriPlayerSeek, isloop?SL_BOOLEAN_TRUE:SL_BOOLEAN_FALSE, 0, SL_TIME_UNKNOWN);
        assert(SL_RESULT_SUCCESS == result);

        isready=true;
        return 0;
    }
    int setplay(bool played)
    {
        if(!isready) return -1;
        SLresult result;
        if (uriPlayerPlay != NULL)
        {
            result = (*uriPlayerPlay)->SetPlayState(uriPlayerPlay, played ?
                                                    SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
            assert(SL_RESULT_SUCCESS == result);
        }
        return 0;
    }
    int start()
    {
        return setplay(true);
    }
    int pause()
    {
        return setplay(false);
    }
    int stop()
    {
        return setplay(false);
    }
    int resume()
    {
        return setplay(true);
    }
    int getstate()
    {
        if(!isready) return -1;
        SLuint32 state = 0;
        (*uriPlayerPlay)->GetPlayState(uriPlayerPlay, &state);
        switch(state)
        {
        case SL_PLAYSTATE_PLAYING:
            return AUDIO_PLAYING;
        case SL_PLAYSTATE_PAUSED:
            return AUDIO_PAUSE;
        case SL_PLAYSTATE_STOPPED:
            return AUDIO_STOP;
        }
        return -9;
    }
    int getdur()
    {
        if(!isready) return -2;
        SLmillisecond dur;
        int kp=-1;
        while(kp<0)
        {
            SLresult ret=(*uriPlayerPlay)->GetDuration(uriPlayerPlay, &dur);
            kp=(int)dur;
            printf("*DBG* %lu %lu %d\n",ret,dur,kp);
            if(ret!=SL_RESULT_SUCCESS) return -502+ret;
        }
        return kp;
    }
    int unload()
    {
        if(!isready) return -1;
        (*uriPlayerObject)->Destroy(uriPlayerObject);
        //(*uriPlayerPlay)->Destroy(uriPlayerPlay);
        //(*uriPlayerSeek)->Destroy(uriPlayerSeek);
        isready=false;
        return 0;
    }
    ~AudioPlayer()
    {
        if(isready) unload();
        (*outputMixObject)->Destroy(outputMixObject);
        (*engineObject)->Destroy(engineObject);
    }
};
