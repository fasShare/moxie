#ifndef MOXIE_POLL_H
#define MOXIE_POLL_H
#include <sys/poll.h>


#include <Poller.h>


#include <boost/noncopyable.hpp>

namespace moxie {

class Poll : public Poller {
public:
    Poll();

    bool EventsAdd(Events* events);
    bool EventsMod(Events* events);
    bool EventsDel(Events* events);
    Timestamp Loop(std::vector<PollerEvent_t> &events, int timeout);

private:
    std::vector<struct pollfd>::iterator getEventPos(const struct pollfd& event);
    static size_t kMaxPollNum_;
    std::vector<struct pollfd> revents_;
};

}
#endif // MOXIE_POLL_H

