#include <EventLoopPool.h>
#include <MutexLocker.h>
#include <Log.h>

fas::EventLoopPool* fas::EventLoopPool::instance_ = nullptr; 
fas::EventLoopPool::EventLoopPool() :
    loops_(),
    nextLoops_(),
    next_(0) {
}

bool fas::EventLoopPool::addEventLoop(long tid, fas::EventLoop* loop, bool ismain) {
    MutexLocker locker(mutex_);
    if ((loop == NULL) || (loops_.find(tid) != loops_.end())) {
        return false;
    }
    loops_[tid] = loop;
    if (!ismain) {
        nextLoops_.push_back(loop);
    }
    LOGGER_TRACE("Num of EventLoop in Bucket is " << nextLoops_.size());
    return true;
}

fas::EventLoop *fas::EventLoopPool::getNextLoop() {
    MutexLocker locker(mutex_);
    if (nextLoops_.size() == 0) {
        return nullptr;
    }
    if (nextLoops_.size() <= next_) {
        next_ = 0;
    }
    return nextLoops_[next_++];
}

fas::EventLoop *fas::EventLoopPool::getLoop(long tid) {
    MutexLocker locker(mutex_);
    auto iter = loops_.find(tid);
    if (iter == loops_.end()) {
        return nullptr;
    }
    return iter->second;
}

fas::EventLoopPool *fas::EventLoopPool::Instance() {
    if (nullptr == instance_) {
        instance_ = new (std::nothrow) EventLoopPool();
    }
    return instance_;
}
