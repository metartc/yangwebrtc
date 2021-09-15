#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

using namespace std;

#include <pthread.h>
#include "yanghttp/YangHttp.h"
#include "yangsql/include/YangConf.h"
#include "yangdataserver/YangDataChannelServer.h"
int g_waitState=0;
pthread_mutex_t g_lock ;//= 0;//PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	g_cond ;//PTHREAD_COND_INITIALIZER;
/*
 * ctrl + c controller
 */
static bool b_exit = 0;
static void ctrl_c_handler(int s){
    printf("\ncaught signal %d, exit.\n",s);
    b_exit = true;
    pthread_mutex_lock(&g_lock);
    pthread_cond_signal(&g_cond);
    pthread_mutex_unlock(&g_lock);
}


static bool b_reload = 0;
static void reload_handler(int s){
    printf("\ncaught signal %d, reload.\n",s);
    b_reload = true;
}



int main(int argc, char* argv[])
{
    struct sigaction    sigIntHandler;
    struct sigaction    sigHupHandler;
    g_lock = PTHREAD_MUTEX_INITIALIZER;
    g_cond = PTHREAD_COND_INITIALIZER;
    //Http Server
    YangConf yconf;
    YangHttp                 http;
    YangDataChannelServer    dataServer;
    yconf.init("yang_config.ini");
    yconf.initValue();
    http.m_conf=&yconf;
    dataServer.m_conf=&yconf;
    YangMapMeeting m_map_meeting   ;//= new YangMapMeeting();
    m_map_meeting.m_usingSrtDataChannel=yconf.m_usingSrtDataChannel;

    //ctrl + c to exit
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, 0);

    //hup to reload
    sigHupHandler.sa_handler = reload_handler;
    sigemptyset(&sigHupHandler.sa_mask);
    sigHupHandler.sa_flags = 0;
    sigaction(SIGHUP, &sigHupHandler, 0);



    http.handle();
    dataServer.init(yconf.m_dataChannelPort);
    dataServer.start();
    //sls manager


	while(!b_exit)
	{


		  g_waitState=1;
		        pthread_cond_wait(&g_cond, &g_lock);
		        g_waitState=0;
		if (b_reload) {
            //reload
    		b_reload = false;

		}
	}

EXIT_PROC:

    dataServer.stop();
    m_map_meeting.clearMap();


    return 0;
}
