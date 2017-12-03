#ifndef FAS_EVENTLOOP_H
#define FAS_EVENTLOOP_H
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <atomic>

#include <PollerFactory.h>
#include <EpollFactory.h>
#include <Mutex.h>
#include <Timestamp.h>
#include <Condition.h>
#include <Poller.h>
#include <TcpHandler.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace fas {

class Events;
class MutexLocker;

class EventLoop : boost::noncopyable {
public:
    using Functor = boost::function<void ()>;
    EventLoop(PollerFactory *pollerFactory);
    EventLoop();
    ~EventLoop();

    long getTid() const;
	void resetOwnerTid();

    bool updateEvents(boost::shared_ptr<Events> event);

    int getEventLoopNum() const;

    bool isInLoopOwnerThread();
    void assertInOwnerThread();  

    void quit();
    bool loop();

    void wakeupLoop();

    bool eventHandleAble(boost::shared_ptr<Events> origin);
private:
    bool pollUpdate(boost::shared_ptr<Events> event);
    void runFunctors();

    static const int kInitMaxEvents_ = 10;
    static std::atomic<int> count_;

    Poller *poll_;
    int pollDelayTime_;
    PollerFactory *pollerFactory_;

    std::vector<PollerEvent_t> revents_;
    std::map<int, boost::shared_ptr<Events>> events_;
    std::vector<boost::shared_ptr<Events>> mutable_;
    
    Mutex mutex_;
    Condition cond_;
    long tid_;

    std::vector<Functor> functors_;
    bool runningFunctors_;

    bool quit_;

    int wakeFd_;
};

int CreateEventfd();

}
#endif // FAS_EVENTLOOP_H
