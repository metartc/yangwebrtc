## YangWebrtc Overview

yangwebrtc是一个自主研发的支持Webrtc/Srt/Rtmp的rtc架构，包含多种视音频编解码和处理等。  

支持视频会议、高清录播直播、直播互动等多种视音频应用。  
可用于远程教育、远程医疗、指挥调度、安防监控、影视录播、协同办公、直播互动等多种行业应用。  
webrtc支持为自主研发，非谷歌lib,兼容webrtc协议 ,可与谷歌Lib和浏览器互通  
支持Srs sfu https://github.com/ossrs/srs/  
支持Linux/Windows操作系统，android/ios/mac版本正开发中  

### yangwebrtc功能

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
 15、64位编程，不支持32位。  

### 目录模块
#### 客户端
YangPushStream webrtc推流系统  
YangPlayer2.0 webrtc拉流播放系统  
yangmeeting2 视频会议  
yangrecord 高清录播直播系统  
yangvrscreen 虚拟和桌面的录制和推流(webrtc/rtmp)  

YangAVLib2.0 yangwebrtc基础类库，实现所有核心功能  
YangMeetingLib2.0 视频会议基础类库  
#### 服务端
YangMeetingServer 视频会议服务端程序  

### yangwebrtc3.0规划  

 1、支持android/ios/mac等。  
 2、webrtc支持h265、av1等。   
 3、传输加密支持国密加密，修改srtp支持国密。  
 4、实现10位、16位全链路打通，从采集、编码、传输、解码、播放这些环节全部支持10/16位。  
  

## Getting Started

Remark: Yangwebrtc2.0 is developing and not stable.  

下载源码，推荐ubuntu或者win10

git clone https://github.com/yangrtc/yangwebrtc.git 


### 服务器部署

启动服务程序，推荐Srs4.0.146及以后版本，下载编译参考http://github.com/ossrs/srs#usage

​      objs/srs -c conf/https.rtc.conf



### 客户端编译

用QT(推荐5.14.2及以上版本)打开工程YangVAlib2.0、YangPushStream、YangPlayer2.0 ,如需改动目录将其.pro文件中HOME_BASE=..\更改为所需目录。  
QT下载地址：https://download.qt.io/archive/qt/5.14/5.14.2/  
静态库需要三个，ssl/crypto/srtp2，目录里已经有编译好的，如果有问题请再编译新的， 然后顺序编译即可。  
Remark:windows下qt编译器为mingw64  
Remark:ubuntu默认不支持opengl sudo apt-get install libgl1-mesa-dev  
编译后的二进制文件在当前bin目录下  
#### debug目录  
linux:bin/app_debug bin/lib_debug  
windows:bin/app_win_debug bin/lib_win_debug  
#### release目录  
linux:bin/app_release bin/lib_release  
windows:bin/app_win_release bin/lib_win_release  

#### windows runtime lib
![图片](https://user-images.githubusercontent.com/87118023/133883028-78abbe6f-4fa1-4817-8ed5-c29951561657.png)
#### linux runtime lib
![图片](https://user-images.githubusercontent.com/87118023/133883160-c10df7ba-ea97-482d-99b4-cb09af73d723.png)

remark:如用qt调试，需将yang_config.ini和lib目录copy到debug和release目录下   
yang_config.ini和lib目录下载链接:https://github.com/yangrtc/yangwebrtc/releases/tag/2.0-runtime   
编译后生成可执行文件放入下载的运行环境即可运行    
 
#### 视频会议YangMeeting2.0编译 

https://github.com/yangrtc/yangwebrtc/wiki/YangMeeting-Getting-Started
  

### 第三方系统源码

https://github.com/yangrtc/yangwebrtc/releases/tag/thirdparty2.0

里面不包含支持vr动态库，如需支持vr，下载opencv3

https://github.com/opencv/opencv/releases/tag/3.4.13 及以上版本

yang_config.ini配置参数https://github.com/yangrtc/yangrtc/wiki/config_file  
### yangwebrtc服务群
![yangwebrtc1](https://user-images.githubusercontent.com/87118023/133881762-a2453c7d-fa8b-4dec-8d1a-084384b23cdd.jpg)
