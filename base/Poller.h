#ifndef FAS_EVENTSPOLLER_H
#define FAS_EVENTSPOLLER_H
#include <vector>


#include <Timestamp.h>
#include <Events.h>


#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace fas {

typedef struct PollerEvent {
    int fd;
    uint32_t event;
} PollerEvent_t;

class Poller : boost::noncopyable {
public:
    enum type {
        EPOLL,
        POLL,
    };
    Poller();
    virtual bool EventsAdd(Events* events) = 0;
    virtual bool EventsMod(Events* events) = 0;
    virtual bool EventsDel(Events* events) = 0;
    virtual Timestamp Loop(std::vector<PollerEvent_t> &events, int timeout) = 0;
};

}
#endif // FAS_EVENTSPOLLER_H
