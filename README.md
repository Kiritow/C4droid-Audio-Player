# C4droid-Audio-Player C4droid音频播放器
###OpenSLES Powered

# ``class AudioPlayer``  音频播放类 AudioPlayer  
**方法 Methods**  
*public*  
**AudioPlayer()**  
>构造播放引擎,初始化引擎  
Initialize a player with engine. 

**_int_ load(_const char*_ URI)**  
>加载音乐资源,准备播放  
Load the music resource, ready to play.  

**_int_ unload()**  
>卸载音乐资源.(析构时将自动进行)  
Unload the music resource.(Called automatically on destroyed)  

**_int_ setplay(_bool_ played)**  
>设置播放状态(true:播放,false:暂停)  
Set play status.(true for play,false for pause)  

**_int_ getstate()**  
>获取播放状态  
Get play status.  

**_int_ getdur_static()**  
>仅在暂停时获取音乐长度(毫秒)  
Get the length of music when the status is "Paused"  

**_int_ getdur_dynamic()**  
>立刻获取音乐长度(毫秒)  
Get the length of music at once.  

**~AudioPlayer()**  
>释放音乐资源,销毁播放引擎.  
Unload the music resource and destroy the engine and the player itself.  


示例代码 Example Code  
```
int main()
{
    AudioPlayer a;
    a.load("example.mp3");
    a.setplay(true);
    usleep(a.getdur_dynamic()*1000);
    return 0;
}
```
