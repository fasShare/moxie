#include <assert.h>
#include <new>

#include <Moxie.h>
#include <Events.h>
#include <EventLoop.h>
#include <EventLoopPool.h>
#include <ThreadPool.h>
#include <HandlePool.h>
#include <TcpHandle.h>
#include <TimerHandle.h>
#include <Eventsops.h>

#include <boost/function.hpp>

moxie::Moxie *moxie::Moxie::moxie_ = nullptr;

void LoopThreadFunc() {
    moxie::EventLoop *loop = new (std::nothrow) moxie::EventLoop();
	if (nullptr == loop) {
		return;
	}

	if (!moxie::EventLoopPool::AddEventLoop(loop)) {
		return;
	}

	loop->loop();
}

moxie::Moxie::Moxie() :
    threadPool_(nullptr),
    loop_(nullptr),
    signor_() {
}

bool moxie::Moxie::MoxieInit(MoxieArgsType args) {
    Instance()->RegisterHandler();
    return Instance()->init(args);
}

bool moxie::Moxie::init(MoxieArgsType args) {
    if (args.InitFunc) {
        args.InitFunc();
    }
    if (args.ThreadNum > 0) {
		threadPool_ = new (std::nothrow) ThreadPool(args.ThreadNum, LoopThreadFunc, "WorkLoopThreadPool");
		assert(Instance()->threadPool_);
		Instance()->threadPool_->start();
    }
	loop_ = new (std::nothrow) EventLoop();
	return EventLoopPool::addMainLoop(loop_);
}

bool moxie::Moxie::Run() {
    return Instance()->loop_->loop();
}

moxie::Moxie* moxie::Moxie::Instance() {
    if (!moxie_) {
        moxie_ = new (std::nothrow) Moxie();
    }
    assert(moxie_);
    return moxie_;
}

void moxie::Moxie::RegisterHandler() {
    Eventsops::RegisterEventHandler(EVENT_TYPE_TCPCON, new TcpHandle());
    Eventsops::RegisterEventHandler(EVENT_TYPE_TIMER, new TimerHandle());
}
