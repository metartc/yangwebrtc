

#include "YangStThread.h"

#include <iostream>
using namespace std;

YangStThread::YangStThread(){
	m_thread=0;
}
YangStThread::~YangStThread(){

}

int YangStThread::start()
{
    if (st_thread_create(YangStThread::go, this, 0, 0))
    {
        cerr << "YangStThread::start could not start thread" << endl;
        return -1;
    }

    return 0;
}

void* YangStThread::go(void* obj)
{
    reinterpret_cast<YangStThread*>(obj)->run();
    return NULL;
}

void* YangStThread::join()
{
    void* ret;
    st_thread_join(m_thread, &ret);
    return ret;
}

st_thread_t YangStThread::getThread()
{
    return m_thread;
}

//int YangStThread::detach()
//{
//    return pthread_detach(m_thread);
//}

//int YangStThread::equals(YangStThread* t)
//{
//    return pthread_equal(m_thread, t->getThread());
//}

void YangStThread::exitThread(void* value_ptr)
{
    pthread_exit(value_ptr);
}

void YangStThread::interrupt()
{
     st_thread_interrupt(m_thread);
}

