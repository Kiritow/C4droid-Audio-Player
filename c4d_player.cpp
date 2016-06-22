#include "jni.h"
#include <SLES/OpenSLES.h>
#include "SLES/OpenSLES_Android.h"
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
public:
    SLObjectItf engineObject;///
    SLEngineItf engineEngine;
    SLObjectItf outputMixObject;///
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
    SLObjectItf uriPlayerObject;///
    SLPlayItf uriPlayerPlay;///
    SLSeekItf uriPlayerSeek;///
    bool isready;
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
    int load(char* uri)
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
        result = (*uriPlayerSeek)->SetLoop(uriPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
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
    int getstate()
    {
        if(!isready) return -1;
        SLuint32 state = 0;
        (*uriPlayerPlay)->GetPlayState(uriPlayerPlay, &state);
        switch(state)
        {
        case SL_PLAYSTATE_PLAYING:
            return 1;
        case SL_PLAYSTATE_PAUSED:
            return 2;
        case SL_PLAYSTATE_STOPPED:
            return 3;
        }
        return -9;
    }
    int getdur_static()
    {
        if(!isready) return -2;
        if(getstate()==1) return -1;
        SLmillisecond dur=0;
        SLresult ret=(*uriPlayerPlay)->GetDuration(uriPlayerPlay, &dur);
        assert(ret==SL_RESULT_SUCCESS);
        //printf("*ret=%d dur=%d\n",ret,dur);
        return dur;
    }
    int getdur_dynamic()
    {
    	  if(!isready) return -2;
        SLmillisecond dur=0;
        SLresult ret=(*uriPlayerPlay)->GetDuration(uriPlayerPlay, &dur);
        assert(ret==SL_RESULT_SUCCESS);
        //printf("*ret=%d dur=%d\n",ret,dur);
        return dur;
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

int main()
{
    AudioPlayer a;
    a.load("/sdcard/2.mp3");
    printf("DUR=%d\n",a.getdur_static());
    printf("State=%d\n",a.getstate());
    a.setplay(true);
    printf("State=%d\n",a.getstate());
    usleep(5000000);
    printf("DUR=%d\n",a.getdur_static());
    printf("%d\n",a.getdur_dynamic());
    usleep(5000000);
    printf("DUR=%d\n",a.getdur_static());
    printf("%d\n",a.getdur_dynamic());
    a.setplay(false);
    printf("State=%d\n",a.getstate());
    printf("DUR=%d\n",a.getdur_static());
    printf("%d\n",a.getdur_dynamic());
}
