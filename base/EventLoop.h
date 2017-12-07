#ifndef MOXIE_EVENTLOOP_H
#define MOXIE_EVENTLOOP_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <atomic>

#include <PollerFactory.h>
#include <EpollFactory.h>
#include <TimerScheduler.h>
#include <Mutex.h>
#include <Timestamp.h>
#include <Condition.h>
#include <Poller.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace moxie {

class Events;
class MutexLocker;

class EventLoop : boost::noncopyable {
public:
    EventLoop(PollerFactory *pollerFactory);
    EventLoop();
    ~EventLoop();

    long getTid() const;
	void resetOwnerTid();

    bool updateEvents(boost::shared_ptr<Events> event);

    int getEventLoopNum() const;
    bool isInLoopOwnerThread();
    void assertInOwnerThread();  

	TimerScheduler* getTimerSchedule();

    void quit();
    bool loop();

    void wakeupLoop();
private:
    bool eventHandleAble(boost::shared_ptr<Events> origin);
    bool pollUpdate(boost::shared_ptr<Events> event);

    static const int kInitMaxEvents_ = 10;
    static std::atomic<int> count_;

    Poller *poll_;
    int pollDelayTime_;
    PollerFactory *pollerFactory_;

    std::map<int, boost::shared_ptr<Events>> events_;
    std::vector<boost::shared_ptr<Events>> mutable_;
    
	TimerScheduler* schedule_;

    Mutex mutex_;
    long tid_;

    bool quit_;
    int wakeFd_;
};

int CreateEventfd();

}
#endif // MOXIE_EVENTLOOP_H
