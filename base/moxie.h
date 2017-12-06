#ifndef MOXIE_MOXIE_H
#define MOXIE_MOXIE_H
#include <SigIgnore.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#define EVENT_TYPE_START        0
#define EVENT_TYPE_TCPCON       1
#define EVENT_TYPE_TIMER        2
#define EVENT_TYPE_TCPSER		3

namespace moxie {

class fas::EventLoop;
class fas::ThreadPool;

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

	ThreadPool *threadPool_;
	EventLoop *loop_;
	Moxie *moxie_;
	SigIgnore signor_;
};

}

#endif //MOXIE_MOXIE_H
