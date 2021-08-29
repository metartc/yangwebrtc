#ifndef YangThread_H__
#define YangThread_H__

#include <pthread.h>

#define yangStopThread(x) if(x){while(x->m_isStart) usleep(1000);}
#define YANG_STOP(x) if(x->m_isStart){x->stop();}
class YangThread {
public:
	YangThread();
	virtual ~YangThread();

	int start();
	void* join();
	int detach();
	int equals(YangThread *t);
	void exitThread(void *value_ptr);
	int cancel();
	pthread_t getThread();
	virtual void stop()=0;
protected:
	virtual void run() = 0;

private:
	static void* go(void *obj);
	pthread_t m_thread;

};

#endif
