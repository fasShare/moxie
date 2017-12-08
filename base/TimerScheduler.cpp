#include <vector>
#include <utility>
#include <algorithm>

#include <TimerScheduler.h>
#include <MutexLocker.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Log.h>

#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

moxie::TimerScheduler::TimerScheduler() :
    timerfd_(::timerfd_create(CLOCK_MONOTONIC, 0)),
    timerheap_(),
    expired_(),
	event_(new Events(timerfd_, kReadEvent)),
    timerCallbackRunning_(false) {

    if (timerfd_ ==  -1) {
        LOGGER_SYSERR("timerfd_create error : " << ::strerror(errno));
    }
	event_->setType(EVENT_TYPE_TIMER);	
	LOGGER_TRACE("Timer fd = " << timerfd_);
}

moxie::TimerHeap::timerfd_t moxie::TimerScheduler::getTimerfd() const {
    return timerfd_;
}

boost::shared_ptr<moxie::Events> moxie::TimerScheduler::getEvent() {
	return event_;
}

struct itimerspec moxie::TimerScheduler::calculateTimerfdNewValue(moxie::Timestamp earlist) {
    int64_t microseconds =  earlist.get_microSecondsSinceEpoch() - \
                            moxie::Timestamp::now().get_microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }

    struct itimerspec newvalue;
    newvalue.it_interval.tv_sec = 0;
    newvalue.it_interval.tv_nsec = 0;

    newvalue.it_value.tv_sec = static_cast<time_t>(microseconds / moxie::Timestamp::kMicroSecondsPerSecond);
    newvalue.it_value.tv_nsec = static_cast<long>((microseconds % moxie::Timestamp::kMicroSecondsPerSecond) * 1000);

    return newvalue;
}

bool moxie::TimerScheduler::resetTimer(moxie::Timestamp earlist) {
    struct itimerspec newvalue;
    struct itimerspec oldvalue;
    bzero(&newvalue, sizeof(struct itimerspec));
    bzero(&oldvalue, sizeof(struct itimerspec));
    newvalue = calculateTimerfdNewValue(earlist);
    int ret = ::timerfd_settime(timerfd_, 0, &newvalue, &oldvalue);
    if (ret < 0) {
        LOGGER_SYSERR("timerfd_settime error : " << ::strerror(errno));
        return false;
    }
    return true;
}

bool moxie::TimerScheduler::addTimer(moxie::Timer *timer) {
	MutexLocker lock(mutex_);
    if (timerCallbackRunning_ == true) {
        auto iter = std::find(expired_.begin(), expired_.end(), \
                std::make_pair(timer->getExpiration(), timer));
        if (iter != expired_.end()) {
            return false;
        }
    }
    timerheap_.addTimer(timer);
    if(timerheap_.getEarlistChange()) {
        Timestamp earlist = timerheap_.getEarlistExpiration();
		LOGGER_WARN("Reset Timer.");
        return resetTimer(earlist);
    }
    return true;
}

void moxie::TimerScheduler::delTimer(moxie::Timer *timer) {
	MutexLocker lock(mutex_);
    if (timerCallbackRunning_ == true) {
        auto iter = std::find(expired_.begin(), expired_.end(), \
                std::make_pair(timer->getExpiration(), timer));
        if (iter != expired_.end()) {
            iter->second->setState(moxie::Timer::STATE::DELETED);
            return;
        }
    }
    timerheap_.delTimer(timer);
    if (timerheap_.getEarlistChange()) {
        Timestamp earlist = timerheap_.getEarlistExpiration();
        resetTimer(earlist);
    }
}

void moxie::TimerScheduler::handleRead(boost::shared_ptr<Events> events, moxie::Timestamp time) {
	MutexLocker lock(mutex_);
    boost::ignore_unused(events, time);
    // now is more accurate than the time of loop wait returned.
    moxie::Timestamp now = moxie::Timestamp::now();
    timerheap_.getExpiredTimers(expired_, now);

	timerCallbackRunning_ = true;
    for (auto iter = expired_.begin(); iter != expired_.end(); iter++) {
        if (iter->second->getState() != moxie::Timer::STATE::DELETED) {
            iter->second->run();
        }
    }
    timerCallbackRunning_ = false;

	timerheap_.restartIntervalTimer(expired_);
    if (timerheap_.getEarlistChange()) {
        resetTimer(timerheap_.getEarlistExpiration());
    }
    expired_.clear();
}

moxie::TimerScheduler::~TimerScheduler() {
    for (auto iter = expired_.begin(); iter != expired_.end(); ++iter) {
        if (iter->second != nullptr) {
            delete iter->second;
            iter->second = nullptr;
        }
    }
}
