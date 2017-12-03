#ifndef FAS_EVENTLOOPPOOL_H
#define FAS_EVENTLOOPPOOL_H
#include <map>
#include <vector>
#include <unistd.h>
#include <atomic>

#include <EventLoop.h>
#include <Mutex.h>

namespace fas {

class EventLoop;

class EventLoopPool {
public:
    static bool AddEventLoop(long tid, EventLoop* loop, bool ismain = false) {
        return Instance()->addEventLoop(tid, loop, ismain);
    }
    static EventLoop *GetNextLoop() {
        return Instance()->getNextLoop();
    }
    static EventLoop *GetLoop(long tid) {
        return Instance()->getLoop(tid);
    }
private:
    bool addEventLoop(long tid, EventLoop* loop, bool ismain = false);
    EventLoop *getNextLoop();
    EventLoop *getLoop(long tid);
    static EventLoopPool *Instance();
    EventLoopPool();
    
    Mutex mutex_;
    std::map<long, EventLoop *> loops_;
    std::vector<EventLoop *> nextLoops_;
    std::atomic<size_t> next_;
    static EventLoopPool *instance_;
};

}

#endif //FAS_EVENTLOOPPOOL_H
