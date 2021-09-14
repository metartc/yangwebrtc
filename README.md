## YangWebrtc Overview

yangwebrtc是一个自主研发的支持Webrtc/Srt/Rtmp的rtc架构，包含多种视音频编解码和处理等。  

支持视频会议、高清录播直播、直播互动等多种视音频应用。  
 可用于远程教育、远程医疗、指挥调度、安防监控、影视录播、协同办公、直播互动等多种行业应用。  
webrtc支持为自主研发，非谷歌lib,兼容webrtc协议 ,可与谷歌Lib和浏览器互通  
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

### 目录功能

yangmeeting 视频会议  
yangrecord 高清录播直播系统  
yangvrscreen 虚拟和桌面的录制和推流(webrtc/rtmp)  
yangplayer 网路播放器(wetrtc/rtmp)  
YangMeetingServer 视频会议服务端程序  

### yangwebrtc3.0规划  

 1、支持android/ios/mac等。  
 2、webrtc支持h265、av1等。   


## Getting Started

下载源码，推荐ubuntu或者win10

git clone https://github.com/yangrtc/yangwebrtc.git 

下载运行环境：在发行版中下载yangwebrtc2.0 runtime，里面包含所需动态库，动态库默认目录名称为lib

https://github.com/yangrtc/yangwebrtc/releases/tag/2.0-runtime

### 服务器部署

可使用ide如eclipse等编译YangMeetingServer，建立mysql数据库yangav,字符集utf-8,从文件yangvd.sql导入数据

数据库参数配置在目录里的yang_config.ini文件中。

启动两个服务程序

1、推荐Srs4.0.146及以后版本，下载编译参考http://github.com/ossrs/srs#usage

​      objs/srs -c conf/https.rtc.conf

2、信令和数据服务程序

​    ./YangMeetingServer

### 客户端编译

用QT打开工程YangVAlib2.0、YangMeetingLib2.0、YangMeeting2.0 ,将其.pro文件中HOME_BASE=..\更改为当前目录。

静态库需要三个，ssl/crypto/srtp2，目录里已经有编译好的，如果有问题请再编译新的， 然后顺序编译即可。

编译后生成可执行文件YangMeeting2或YangMeeting2.exe 放入下载的运行环境，修改配置文件yang_config.ini  
rtcServerIP=10.42.0.1  
httpServerIP=10.42.0.1  
dataServerIP=10.42.0.1  
将这三个地址配置为服务器地址,然后点击应用程序即可运行。
默认用户有admin密码123
用户a01 密码123

### linux部署图   
![图片](https://user-images.githubusercontent.com/87118023/133176974-b8a0c7b3-2eb3-4cc4-9324-a6c4547a4a42.png)

### windows部署图  
![图片](https://user-images.githubusercontent.com/87118023/133177771-3c512f7a-46bf-4a36-b1ce-1a3e2ea43940.png)


  

### 第三方系统源码

https://github.com/yangrtc/yangwebrtc/releases/tag/thirdparty2.0

里面不包含支持vr动态库，如需支持vr，下载opencv3

https://github.com/opencv/opencv/releases/tag/3.4.13 及以上版本

yang_config.ini配置参数https://github.com/yangrtc/yangrtc/wiki/config_file  
