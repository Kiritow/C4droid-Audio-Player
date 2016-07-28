#include <windows.h>
#include <mmsystem.h>
#include <cstdlib>
#include <cstdio>
using namespace std;

class AudioPlayer
{
private:
    MCI_OPEN_PARMS op;
    void resetdevice()
    {
        op.dwCallback=(DWORD)NULL;
        op.lpstrAlias=NULL;
        op.lpstrDeviceType="MPEGAudio";
        op.wDeviceID=(DWORD)NULL;
        mURI=nullptr;
    }
    bool isPlaying;
    bool isReady;
    bool isSeeked;
    char* mURI;
    int _win_play()
    {
        if(isPlaying) return -503;
        MCI_PLAY_PARMS parm;
        parm.dwFrom=getcur();
        UINT rs=mciSendCommand(op.wDeviceID,MCI_PLAY,0,(DWORD)&parm);
        if(rs==0)
        {
            isPlaying=true;
        }
        return rs;
    }
public:
    AudioPlayer()
    {
        resetdevice();
        isPlaying=false;
        isSeeked=false;
        isReady=false;
    }
    int load(const char* URI,bool isloop)
    {
        if(isReady) return -2;
        mURI=new char[strlen(URI)+8];
        memset(mURI,0,strlen(URI)+8);
        strcpy(mURI,URI);
        op.lpstrElementName=mURI;
        UINT rs=mciSendCommand((DWORD)NULL,MCI_OPEN,MCI_OPEN_ELEMENT,(DWORD)&op);
        if(rs==0)
        {
            isReady=true;
        }
        return rs;
    }
    int unload()
    {
        if(!isReady) return -503;
        if(isPlaying) stop();
        UINT rs=mciSendCommand(op.wDeviceID, MCI_CLOSE, (DWORD)NULL,(DWORD) NULL);
        if(rs!=0) return rs;
        delete[] mURI;
        resetdevice();
        return 0;
    }
    int start()
    {
        if(isPlaying) return -503;
        MCI_PLAY_PARMS parm;
        parm.dwFrom=0;
        UINT wDeviceID=op.wDeviceID;
        UINT rs=mciSendCommand(wDeviceID,MCI_PLAY,0,(DWORD)&parm);
        if(rs==0)
        {
            isPlaying=true;
        }
        return rs;
    }
    int pause()
    {
        if(!isPlaying) return -503;
        MCI_PLAY_PARMS parm;
        UINT rs=mciSendCommand(op.wDeviceID,MCI_PAUSE,0,(DWORD)&parm);
        if(rs==0)
        {
            isPlaying=false;
        }
        return rs;
    }
    int resume()
    {
        if(isPlaying) return -503;
        if(isSeeked)
        {
            _win_play();
            isSeeked=false;
        }
        MCI_PLAY_PARMS parm;
        UINT rs=mciSendCommand(op.wDeviceID,MCI_RESUME,0,(DWORD)&parm);
        if(rs==0)
        {
            isPlaying=true;
        }
        return rs;
    }
    int stop()
    {
        if(!isPlaying) return -503;
        UINT rs=mciSendCommand(op.wDeviceID,MCI_STOP,(DWORD)NULL,(DWORD)NULL);
        if(rs==0)
        {
            isPlaying=false;
        }
        return rs;
    }
    int getstate()
    {
        if(!isReady) return -404;
        MCI_STATUS_PARMS StatusParms;
        StatusParms.dwItem = MCI_STATUS_MODE;
        UINT rs=mciSendCommand(op.wDeviceID, MCI_STATUS,MCI_WAIT | MCI_STATUS_ITEM,(DWORD)&StatusParms);
        if(rs!=0) return rs;
        switch(StatusParms.dwReturn)
        {
            case MCI_MODE_PLAY: return AUDIO_PLAYING;
            case MCI_MODE_PAUSE: return AUDIO_PAUSE;
            case MCI_MODE_STOP: return AUDIO_STOP;
            default: return StatusParms.dwReturn;
        }
    }
    int getdur()
    {
        if(!isReady) return -404;
        MCI_STATUS_PARMS StatusParms;
        StatusParms.dwItem = MCI_STATUS_LENGTH;
        UINT rs=mciSendCommand(op.wDeviceID, MCI_STATUS,MCI_WAIT | MCI_STATUS_ITEM,(DWORD)&StatusParms);
        if(rs!=0) return rs;
        return StatusParms.dwReturn;
    }
    int getcur()
    {
        if(!isReady) return -404;
        MCI_STATUS_PARMS StatusParms;
        StatusParms.dwItem = MCI_STATUS_POSITION;
        UINT rs=mciSendCommand(op.wDeviceID, MCI_STATUS,MCI_WAIT | MCI_STATUS_ITEM,(DWORD)&StatusParms);
        if(rs!=0) return rs;
        return StatusParms.dwReturn;
    }
    int seekto(int milliSecond)
    {
        if(!isReady) return -404;
        MCI_SEEK_PARMS parm;
        parm.dwTo=milliSecond;
        UINT rs=mciSendCommand(op.wDeviceID, MCI_SEEK,MCI_TO|MCI_WAIT,(DWORD)&parm);
        if(rs==0)
        {
            isPlaying=false;isSeeked=true;
        }
        return rs;
    }
    int seekto(int Minute,int Second)
    {
        return seekto((Minute*60+Second)*1000);
    }
    ~AudioPlayer()
    {
        if(isReady) unload();
    }
};
