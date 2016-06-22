/// Windows Platform : CPPLIB Audio Implements.

#if defined( __WIN32__ ) || !defined(__C4DROID__)
class AudioPlayer
{
public:
    ///构造播放引擎,初始化引擎
    AudioPlayer();
    ///加载音乐资源,准备播放
    int load(const char* URI);
    ///卸载音乐资源.(析构时将自动进行)
    int unload();
    ///设置播放状态(true:播放,false:暂停)
    int setplay(bool played);
    ///获取播放状态
    int getstate();
    ///仅在暂停时获取音乐长度(毫秒)
    int getdur_static();
    ///立刻获取音乐长度(毫秒)
    int getdur_dynamic();
    ///释放音乐资源,销毁播放引擎.
    ~AudioPlayer();
};
#else /// C4droid
#include "c4d_player.hpp"
#endif
