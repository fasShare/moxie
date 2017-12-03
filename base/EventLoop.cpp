#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <new>

#include <Log.h>
#include <Poller.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Events.h>
#include <Thread.h>
#include <TcpHandle.h>
#include <EventfdHandle.h>
#include <MutexLocker.h>
#include <Eventsops.h>

#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

std::atomic<int> fas::EventLoop::count_(0);

fas::EventLoop::EventLoop() :
    EventLoop(new (std::nothrow) EpollFactory) {
}

fas::EventLoop::EventLoop(PollerFactory *pollerFactory) :
    poll_(nullptr),
    pollDelayTime_(200),
    pollerFactory_(pollerFactory),
    events_(),
    mutable_(),
    mutex_(),
    tid_(gettid()),
    quit_(false),
    wakeFd_(CreateEventfd()) {
    poll_ = pollerFactory_->getPoller();
    if (!poll_) {
        LOGGER_SYSERR("New Poller error!");
    }
    count_++;
    Eventsops::RegisterEventHandler(Events::type::EVENTFD, new EventfdHandle());
    Eventsops::BuildEventIntoLoop(this, new Events(wakeFd_, kReadEvent), Events::type::EVENTFD);
}

int fas::CreateEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOGGER_SYSERR("Failed in eventfd!");
        ::abort();
    }   
    return evtfd;
}

long fas::EventLoop::getTid() const{
    return tid_;
}

int fas::EventLoop::getEventLoopNum() const {
    return count_;
}

bool fas::EventLoop::updateEvents(boost::shared_ptr<Events> event) {
    MutexLocker lock(mutex_);
    LOGGER_TRACE("updateEvents start");
    if (event->invaild()) {
        LOGGER_TRACE("updateEvents event invaild");
        return false;
    }
    if (!(tid_ == gettid())) {
        wakeupLoop();
        mutable_.push_back(event);
        LOGGER_TRACE("updateEvents not in same thread.");
        return true;
    }

    return pollUpdate(event);
}

void fas::EventLoop::wakeupLoop() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOGGER_ERROR("EventLoop::wakeup() writes " << n << " bytes instead of 8");
    }
}

bool fas::EventLoop::pollUpdate(boost::shared_ptr<fas::Events> event) {
    LOGGER_TRACE("pollUpdate start");
    if (event->invaild()) {
        return false;
    }

    if (event->isnew()) {
        auto iter = events_.find(event->getFd()); 
        assert(iter == events_.end());
        events_[event->getFd()] = event;
        LOGGER_TRACE("pollUpdate new");
        return poll_->EventsAdd(event.get());
    } else if (event->ismod()) {
        LOGGER_TRACE("pollUpdate mod");
        return poll_->EventsMod(event.get());
    } else if (event->isdel()) {
        LOGGER_TRACE("pollUpdate del");
        poll_->EventsDel(event.get());
        assert(events_.erase(event->getFd()));
    }
    LOGGER_TRACE("pollUpdate end");
    return true;
}

void fas::EventLoop::resetOwnerTid() {
	tid_ = gettid();
}

void fas::EventLoop::assertInOwnerThread() {
    assert(gettid() == tid_);
}

void fas::EventLoop::quit() {
    MutexLocker lock(mutex_);
    quit_ = true;
    if(!(tid_ == gettid())) {
        wakeupLoop();
    }
    boost::ignore_unused(lock);
}

bool fas::EventLoop::eventHandleAble(boost::shared_ptr<Events> origin) {
    if (origin->isdel()) {
        return false;
    }
    if (origin->ismod()) {
        if (origin->isRead() && (!origin->originRead())) {
            origin->deleteEventMutable(kReadEvent);
        }
        if (origin->isWrite() && (!origin->originWrite())) {
            origin->deleteEventMutable(kWriteEvent);
        }
        return origin->getMutable() != kNoneEvent;
    }
    return true;
}

bool fas::EventLoop::loop() {
    LOGGER_TRACE("A EventLoop started.");
    assertInOwnerThread();
    Timestamp looptime;
    std::vector<PollerEvent_t> occur;
    while (!quit_) {
        LOGGER_TRACE("start new loopping.");
        occur.clear();
        decltype(mutable_) mutable_swap;
        {
            MutexLocker lock(mutex_);
            mutable_swap.swap(mutable_);
        }
        for (auto iter = mutable_swap.begin(); iter != mutable_swap.end(); ++iter) {
            LOGGER_TRACE("update enent.");
            pollUpdate(*iter);
        }

        looptime = poll_->Loop(occur, 10000000);
        for(auto iter = occur.begin(); iter != occur.end(); iter++) {
            auto events = events_.find(iter->fd);
            if (events == events_.end()) {
                continue;
            }
            events->second->setMutable(iter->event);
            if (!eventHandleAble(events->second)) {
                continue;
            }
            Handler * handler = Eventsops::EventHandler(events->second->getType());
            assert(handler);
            handler->doHandle(events->second, looptime);
        }
    } 
    return true;
}

fas::EventLoop::~EventLoop() {
    delete pollerFactory_;
    LOGGER_TRACE("EventLoop will be destroyed in Thread:" << gettid());
}

