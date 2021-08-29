#ifndef SRC_YANGUTIL_SRC_YANGTIMER_H_
#define SRC_YANGUTIL_SRC_YANGTIMER_H_
#include <yangutil/sys/YangThread.h>
class YangTimerTask {
public:
	YangTimerTask() {};
	virtual ~YangTimerTask() {};
	virtual void doTask(int taskId)=0;
};


class YangTimer: public YangThread {
public:
	YangTimer();
	virtual ~YangTimer();
	int m_isStart;
	int m_loop;
	int m_waitState;
	int m_waitTime;

	void setTask(YangTimerTask *ptask);
	void setTimelen(int ptimelen);//hao miao


	void setTaskId(int ptaskId);
	void stop();
protected:
	void run();
	void startLoop();
	void stopLoop();
private:
	YangTimerTask *m_task;
	pthread_mutex_t m_lock;
	pthread_cond_t m_cond_mess;
	int m_taskId;
};

#endif /* SRC_YANGUTIL_SRC_YANGTIMER_H_ */
