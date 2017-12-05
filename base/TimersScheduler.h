#ifndef FAS_TIMERSSCHEDULER_H
#define FAS_TIMERSSCHEDULER_H
#include <sys/timerfd.h>

#include <TimerHeap.h>
#include <Timestamp.h>
#include <Events.h>
#include <Handler.h>

namespace fas {

class TimersScheduler : public handler{
public:
    TimersScheduler();
    ~TimersScheduler();

    TimerHeap::timerfd_t getTimerfd() const;

    bool addTimer(Timer *);
    void delTimer(Timer *);

    void handleRead(boost::shared_ptr<Events> event, Timestamp time);
    void handleWrite(boost::shared_ptr<Events> event, Timestamp time);
    void handleError(boost::shared_ptr<Events> event, Timestamp time);
    void handleClose(boost::shared_ptr<Events> event, Timestamp time);
private:
    struct itimerspec calculateTimerfdNewValue(Timestamp earlist);
    bool resetTimer(Timestamp earlist);

    TimerHeap::timerfd_t timerfd_;
    TimerHeap timerheap_;
    std::vector<std::pair<Timestamp, Timer*>> expired_;
    bool timerCallbackRunning_;
};

}
#endif // TIMERSSCHEDULER_H

