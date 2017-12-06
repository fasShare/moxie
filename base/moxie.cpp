#include <assert.h>
#include <new>

#include <Moxie.h>
#include <EventLoopPool.h>
#include <ThreadPool.h>
#include <HandlePool.h>
#include <TcpHandle.h>
#include <TimerHandler.h>

#include <boost/function.h>

moxie::Moxie *moxie::Moxie::moxie_ = nullptr;

void LoopThreadFunc() {
	EventLoop *loop = new (std::nothrow) EventLoop();
	if (nullptr == loop) {
		return;
	}

	if (!EventLoopPool::AddEventLoop(gettid(), loop)) {
		return;
	}

	loop->loop();
}

bool moxie::Moxie::MoxieInit(MoxieArgsType args) {
    Instance()->RegisterHandler();
    if (args.InitFunc) {
        args.InitFunc();
    }
    if (args.ThreadNum > 0) {
		threadPool_ = new (std::nothrow) ThreadPool(args.ThreadNum, LoopThreadFunc, "WorkLoopThreadPool");
		assert(threadPool_);
		threadPool_->start();
    }
	loop_ = new EventLoop();
	fas::EventLoopPool::addMainLoop(loop_);
}

bool moxie::Moxie::Run() {
	loop_->loop();
}

static moxie::Moxie* Moxie::Instance() {
    if (!moxie_) {
        moxie_ = new (nothrow) Moxie();
    }
    assert(moxie_);
    return moxie_;
}

void moxie::Moxie::RegisterHandler() {
    Eventsops::RegisterEventHandler(EVENT_TYPE_TCPCON, new TcpHandle());
    Eventsops::RegisterEventHandler(EVENT_TYPE_TIMER, new TimerHandle());
}
