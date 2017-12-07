#ifndef MOXIE_TIMERSCHEDULER_H
#define MOXIE_TIMERSCHEDULER_H
#include <sys/timerfd.h>

#include <TimerHeap.h>
#include <Timestamp.h>
#include <Events.h>
#include <Timer.h>

namespace moxie {

class TimerScheduler {
public:
    TimerScheduler();
    ~TimerScheduler();

    TimerHeap::timerfd_t getTimerfd() const;
	boost::shared_ptr<Events> getEvent();

    bool addTimer(Timer *);
    void delTimer(Timer *);

    void handleRead(boost::shared_ptr<Events> event, Timestamp time);
private:
    struct itimerspec calculateTimerfdNewValue(Timestamp earlist);
    bool resetTimer(Timestamp earlist);

    TimerHeap::timerfd_t timerfd_;
    TimerHeap timerheap_;
    std::vector<std::pair<Timestamp, Timer*>> expired_;
	boost::shared_ptr<Events> event_;
    bool timerCallbackRunning_;
};

}
#endif // TIMERSCHEDULER_H

