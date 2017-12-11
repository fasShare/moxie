#ifndef MOXIE_MOXIE_H
#define MOXIE_MOXIE_H
#include <SigIgnore.h>
#include <MoxieConf.h>

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
};

}

#endif //MOXIE_MOXIE_H
