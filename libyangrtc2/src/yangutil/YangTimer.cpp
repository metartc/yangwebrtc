
#include <yangutil/sys/YangTimer.h>
#include <yangutil/sys/YangTime.h>
#include <yangutil/yangtype.h>
#ifndef _WIN32
#include <sys/time.h>
#endif
#include <stdio.h>
#include <yangutil/yang_unistd.h>
YangTimer::YangTimer() {
	m_loop = 0;
	m_isStart = 0;
	m_waitState=0;
	m_waitTime=100;
#if Yang_Using_Phtread
    pthread_mutex_init(&m_lock,NULL);
    pthread_cond_init(&m_cond_mess,NULL);
	//m_lock = PTHREAD_MUTEX_INITIALIZER;
	//m_cond_mess = PTHREAD_COND_INITIALIZER;
#endif
	m_task=NULL;
	m_taskId=0;
}

YangTimer::~YangTimer() {

	if(m_isStart){
		stop();
        while(m_isStart)	yang_usleep(1000);
	}
	m_task=NULL;
#if Yang_Using_Phtread
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_cond_mess);
#endif
}
void YangTimer::setTaskId(int32_t ptaskId){
	m_taskId=ptaskId;
}
void YangTimer::setTask(YangTimerTask *ptask){
	m_task=ptask;
}
void YangTimer::setTimelen(int32_t ptimelen){
	m_waitTime=ptimelen;
}
void YangTimer::run() {
	m_isStart = 1;
    startLoop();
	m_isStart = 0;
}
void YangTimer::stop() {
	stopLoop();

}
void YangTimer::stopLoop() {
	m_loop = 0;
	//pthread_mutex_unlock(&m_lock);
#if Yang_Using_Phtread
	if(m_waitState){
        pthread_mutex_lock(&m_lock);
        pthread_cond_signal(&m_cond_mess);
        pthread_mutex_unlock(&m_lock);

	}
#endif
}

void YangTimer::startLoop() {
   m_loop = 1;
    //pthread_mutex_unlock(&m_lock);
#if Yang_Using_Phtread
    struct timespec outtime;

   struct timeval now;
   pthread_mutex_lock(&m_lock);
#endif
    while (m_loop) {
#if Yang_Using_Phtread
       gettimeofday(&now, NULL);

       long nsec = now.tv_usec * 1000 + (m_waitTime % 1000) * 1000000;
       outtime.tv_sec=now.tv_sec + nsec / 1000000000 + m_waitTime / 1000;
       outtime.tv_nsec=nsec % 1000000000;

        m_waitState=1;

        pthread_cond_timedwait(&m_cond_mess, &m_lock,&outtime);
        m_waitState=0;
#else
        yang_sleep(m_waitTime*1000);
#endif
        if(m_task) m_task->doTask(m_taskId);

    }
#if Yang_Using_Phtread
    pthread_mutex_unlock(&m_lock);
#endif
}
