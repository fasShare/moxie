#ifndef MOXIE_MOXIE_H
#define MOXIE_MOXIE_H
#include <SigIgnore.h>
#include <MoxieConf.h>
#include <Condition.h>
#include <Mutex.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace moxie {

class EventLoop;
class ThreadPool;	

class Moxie {
public:
    static bool MoxieInit(const MoxieConf& conf);
    static const MoxieConf& getConf();
	static bool Run();
    static void ThreadRunNotify();
    static void ThreadRunWait();
private:
    static Moxie* Instance();
    void RegisterHandler();
    bool init(const MoxieConf& conf);
    Moxie();

	ThreadPool *threadPool_;
	EventLoop *loop_;
	static Moxie *moxie_;
	SigIgnore signor_;
    MoxieConf conf_;
    Mutex mutex_;
    Condition cond_;
};

}

#endif //MOXIE_MOXIE_H
