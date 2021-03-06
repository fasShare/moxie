#include <assert.h>
#include <new>
#include <atomic>

#include <Moxie.h>
#include <Events.h>
#include <EventLoop.h>
#include <EventLoopPool.h>
#include <ThreadPool.h>
#include <HandlePool.h>
#include <TcpHandle.h>
#include <TimerHandle.h>
#include <EventfdHandle.h>
#include <ServiceManager.h>
#include <Eventsops.h>

#include <boost/function.hpp>

moxie::Moxie *moxie::Moxie::moxie_ = nullptr;

static std::atomic<int> start_num; 

void LoopThreadFunc() {
    moxie::ServiceManager manager(moxie::Moxie::getConf().getAddress());
    if (!manager.createClientForThisThread()) {
        LOGGER_ERROR("Create clients error in this thread.");
        return;
    }
    moxie::EventLoop *loop = new (std::nothrow) moxie::EventLoop();
	if (nullptr == loop) {
		return;
	}

	if (!moxie::EventLoopPool::AddEventLoop(loop)) {
		return;
	}

    ++start_num;
    moxie::Moxie::ThreadRunNotify();

	loop->loop();
}

moxie::Moxie::Moxie() :
    threadPool_(nullptr),
    loop_(nullptr),
    signor_(),
    mutex_(),
    cond_(mutex_) {
}

const moxie::MoxieConf& moxie::Moxie::getConf() {
    return Instance()->conf_;
}

bool moxie::Moxie::MoxieInit(const MoxieConf& conf) {
    Instance()->conf_ = conf;
    Instance()->RegisterHandler();
    return Instance()->init(conf);
}

bool moxie::Moxie::init(const MoxieConf& conf) {
    // init thread pool
	int ThreadNum = conf.getThreadNum();
    if (ThreadNum > 0) {
		threadPool_ = new (std::nothrow) ThreadPool(ThreadNum, LoopThreadFunc, "WorkLoopThreadPool");
		assert(Instance()->threadPool_);
    }
	// init logger
	CommonLog::OpenLog(conf.getLogConf());
	// init eventloop
	loop_ = new (std::nothrow) EventLoop();
	return EventLoopPool::addMainLoop(loop_);
}

bool moxie::Moxie::Run() {
    ServiceManager manager(getConf().getAddress());
    if (!manager.createClientForThisThread()) {
        LOGGER_ERROR("Create clients error in this thread.");
        return false;
    }
	if (Instance()->threadPool_) {
		Instance()->threadPool_->start();
	}

    int ThreadNum = Moxie::getConf().getThreadNum();

    while (start_num != ThreadNum) {
        LOGGER_WARN("Started thread num not equal required thread num....");
        ThreadRunWait();
    }

    return Instance()->loop_->loop();
}

void moxie::Moxie::ThreadRunNotify() {
    Instance()->cond_.notify();
}

void moxie::Moxie::ThreadRunWait() {
    Instance()->cond_.wait();
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
    Eventsops::RegisterEventHandler(EVENT_TYPE_EVENT, new EventfdHandle());
}
