# C4droid-Audio-Player C4droid音频播放器
###OpenSLES Powered

# 音频播放类  ``class AudioPlayer`` 
**方法 Methods**  
*public*  
**AudioPlayer()** 构造播放引擎,初始化引擎  
**_int_ load(_const char*_ URI)** 加载音乐资源,准备播放  
**_int_ unload()** 卸载音乐资源.(析构时将自动进行)  
**_int_ setplay(_bool_ played)** 设置播放状态(true:播放,false:暂停)  
**_int_ getstate()** 获取播放状态  
**_int_ getdur_static()** 仅在暂停时获取音乐长度(毫秒)  
**_int_ getdur_dynamic()** 立刻获取音乐长度(毫秒)  
**~AudioPlayer()** 释放音乐资源,销毁播放引擎.  

示例代码  
```
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
```
