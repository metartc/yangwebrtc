## MetaRTC(yangrtc) Overview

MetaRTC(yangrtc)是一个自主研发的支持Webrtc/Srt/Rtmp的rtc架构，包含多种视音频编解码和处理等。  

支持视频会议、高清录播直播、直播互动、云游戏、云3D等多种视音频应用。  
可用于远程教育、远程医疗、指挥调度、安防监控、影视录播、协同办公、直播互动等多种行业应用。  
webrtc支持为自主研发，非谷歌lib,兼容webrtc协议 ,可与谷歌Lib和浏览器互通  
支持Srs sfu https://github.com/ossrs/srs/  
支持Linux/Windows操作系统，android/ios/mac版本正开发中  

## MetaRTC(yangrtc)微信群
加微信taihang82

### MetaRTC(yangrtc)功能

 1、视频编码 8bit:x264、x265、vaapi、nvenc等，二期增加AV1和多种硬件编码。  
 2、视频编码 10bit:x265、vaapi、nvenc等。  
 3、视频解码：ffmpeg和yangh264decoder。  
 4、VR:基于抠图实现虚拟视频的互动和录制、直播等。  
 5、8bit和10bit网络播放器：yangplayer  
 6、音频：Opus、Aac、Speex、Mp3等音频编解码。  
 7、音频：AEC、AGC、ANS及声音合成等处理。  
 8、传输：webrtc、rtmp、srt，webrtc为自己实现，没使用谷歌lib库。  
 9、直播：rtmp、srt、webrtc、HLS、HTTP-FLV。  
 10、8bit录制：h264、h265的mp4和flv。  
 11、10bit录制：h265的mp4  
 12、实现了屏幕共享与控制。  
 13、实现了声音和图像多种处理。  
 14、专业摄像头的云台控制与多镜头导播切换。  
  

### 目录简介
#### libyangrtc2 
yangrtc基础类库，实现所有核心功能  
#### yangpushstream2 
webrtc推流系统demo  
#### yangplayer2 
webrtc拉流播放系统demo 

  
## Getting Started

## 一定要看编译教程视频，不看一定会扑街！！！ 


下载源码，推荐ubuntu或者win10

```
git clone https://github.com/metartc/metaRTC.git 
```

### 服务器部署

启动srs服务程序，下载编译参考`http://github.com/ossrs/srs#usage`
#### srs下载地址

```
https://github.com/ossrs/srs/releases/  
​objs/srs -c conf/https.rtc.conf  
​objs/srs -c conf/rtc.conf
```


### 客户端编译

#### 编译教程视频
`https://www.bilibili.com/video/BV1d3411r742/`
`https://github.com/metartc/yangrtc2/releases/tag/2.0.032_video`  
#### 编译教程
gpu编码器搭建游戏教育等在线低延迟直播 https://blog.csdn.net/m0_56595685/article/details/121575105   
搭建兼容webrtc的跨平台的云桌面  https://blog.csdn.net/m0_56595685/article/details/121410868  
搭建虚拟背景和美颜webrtc直播系统 https://blog.csdn.net/m0_56595685/article/details/121575105  
搭建视频会议 https://blog.csdn.net/m0_56595685/article/details/121304109  
搭建H264和H265的MP4录制系统  https://blog.csdn.net/m0_56595685/article/details/121720754  
搭建H265（HEVC）的webrtc应用 https://blog.csdn.net/m0_56595685/article/details/121880362  
#### 视频会议编译教程
https://blog.csdn.net/m0_56595685/article/details/121304109
#### 编译
- 用QT(推荐5.14.2及以上版本)打开工程`libyangrtc2、yangpushstream2、yangplayer2。    
[QT下载地址：](https://download.qt.io/archive/qt/5.14/5.14.2/)  
静态库需要三个，ssl/crypto/srtp2，目录里已经有编译好的，如果有问题请再编译新的， 然后顺序编译即可。  
Remark:ubuntu默认不支持opengl 
```
sudo apt update
sudo apt-get install libgl1-mesa-dev  
```
编译后的二进制文件在当前bin目录下  
#### debug目录  
linux:bin/app_debug bin/lib_debug  
windows:bin/app_win_debug bin/lib_win_debug  
#### release目录  
linux:bin/app_release bin/lib_release  
windows:bin/app_win_release bin/lib_win_release  

 **remark:程序需要一些动态库才能运行，所需动态库在lib文件夹里，lib文件夹在runtime包中  **   
 **remark:如用qt调试，需将yang_config.ini和lib目录copy到debug和release目录下  **  
#### windows qt debug dlls
![图片](https://user-images.githubusercontent.com/87118023/136520546-a03812a7-f91a-479d-a1ff-f6771829d202.png)

#### windows lib dlls
![图片](https://user-images.githubusercontent.com/87118023/133883028-78abbe6f-4fa1-4817-8ed5-c29951561657.png)
#### linux lib so
![图片](https://user-images.githubusercontent.com/87118023/133883160-c10df7ba-ea97-482d-99b4-cb09af73d723.png)


编译后生成可执行文件放入下载的运行环境即可运行      

### 第三方系统源码

https://github.com/metartc/yangwebrtc/releases/tag/thirdparty2.0

里面不包含支持vr动态库，如需支持vr，下载opencv3

https://github.com/opencv/opencv/releases/tag/3.4.13 及以上版本

yang_config.ini配置参数https://github.com/metartc/yangrtc/wiki/config_file  


