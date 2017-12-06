#ifndef MOXIE_MOXIE_H
#define MOXIE_MOXIE_H
#include <SigIgnore.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace moxie {

class EventLoop;
class ThreadPool;

void MoxieDefaultInit() {
}

typedef struct moxie_args_t{
	int ThreadNum;
	boost::function<void ()> InitFunc;

	moxie_args_t () {
		ThreadNum = 1;
		InitFunc = MoxieDefaultInit;
	}
} MoxieArgsType;	

class Moxie {
public:
    static bool MoxieInit(MoxieArgsType args);
	static bool Run();
private:
    static Moxie* Instance();
    void RegisterHandler();
    bool init(MoxieArgsType args);
    Moxie();

	ThreadPool *threadPool_;
	EventLoop *loop_;
	static Moxie *moxie_;
	SigIgnore signor_;
};

}

#endif //MOXIE_MOXIE_H
