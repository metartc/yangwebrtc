
#ifndef YangStThread_H__
#define YangStThread_H__

#include <st.h>


class YangStThread
{
    public:
	YangStThread();
	virtual ~YangStThread();

        int start();
        void* join();
      //  int detach();
      //  int equals(YangStThread* t);
        void exitThread(void* value_ptr);
        void interrupt();
        st_thread_t getThread();
        virtual void stop()=0;
    protected:
        virtual void run() = 0;

    private:
        static void* go(void* obj);
        st_thread_t m_thread;

};

#endif
