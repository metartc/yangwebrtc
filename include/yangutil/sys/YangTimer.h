#ifndef SRC_YANGUTIL_SRC_YANGTIMER_H_
#define SRC_YANGUTIL_SRC_YANGTIMER_H_
#include <yangutil/sys/YangThread.h>
class YangTimerTask {
public:
	YangTimerTask() {};
	virtual ~YangTimerTask() {};
	virtual void doTask(int32_t taskId)=0;
};


class YangTimer: public YangThread {
public:
	YangTimer();
	virtual ~YangTimer();
	int32_t m_isStart;
	int32_t m_loop;
	int32_t m_waitState;
	int32_t m_waitTime;

	void setTask(YangTimerTask *ptask);
	void setTimelen(int32_t ptimelen);//hao miao


	void setTaskId(int32_t ptaskId);
	void stop();
protected:
	void run();
	void startLoop();
	void stopLoop();
private:
	YangTimerTask *m_task;
	pthread_mutex_t m_lock;
	pthread_cond_t m_cond_mess;
	int32_t m_taskId;
};

#endif /* SRC_YANGUTIL_SRC_YANGTIMER_H_ */
