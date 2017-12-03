#ifndef FAS_EVENTS_H
#define FAS_EVENTS_H
#include <iostream>
#include <sys/epoll.h>
#include <sys/poll.h>

#include <Log.h>

#include <boost/static_assert.hpp>

namespace fas {

BOOST_STATIC_ASSERT_MSG(EPOLLIN == POLLIN, "EPOLLIN != POLLIN");
BOOST_STATIC_ASSERT_MSG(EPOLLOUT == POLLOUT, "EPOLLOUT != POLLOUT");
BOOST_STATIC_ASSERT_MSG(EPOLLPRI == POLLPRI, "EPOLLPRI != POLLPRI");
BOOST_STATIC_ASSERT_MSG(EPOLLERR == POLLERR, "EPOLLERR != POLLERR");
BOOST_STATIC_ASSERT_MSG(EPOLLRDHUP == POLLRDHUP, "EPOLLRDHUP != POLLRDHUP");

const uint32_t kNoneEvent = 0;
const uint32_t kReadEvent = POLLIN | POLLPRI;
const uint32_t kWriteEvent = POLLOUT;

class Events {
public:
    enum state {
        NEW,
        MOD,
        USED,
        DEL,
        INVAILD,
    };
    enum type {
        TCP,
        TCP_SERVER,
        TIMER,
        SIGNAL,
        EVENT_FD,
        TYPEINVAILD,
    };

    Events(const int fd = -1, uint32_t events = kNoneEvent);
    Events(const Events& events);
    Events(const struct epoll_event& events);
    Events(const struct pollfd& events);
    ~Events();

    bool reset();

    bool invaild() const;
    bool ismod() const;
    bool isnew() const;
    bool isused() const;
    bool isdel() const;

    bool isRead();
    bool isWrite();
    bool isError();
    bool isClose();

    bool originRead();
    bool originWrite();

    void setType(type tp);
    type getType() const;

    int getFd() const;
    void setFd(int fd);
    
    state getState() const;
    void setState(state sta);

    uint32_t getEvents() const;
    uint32_t getMutable() const;
    void setMutable(uint32_t event);

    void updateEvents(uint32_t events);
    void addEvent(uint32_t event);
    void deleteEvent(uint32_t event);
    void addEventMutable(uint32_t event);
    void deleteEventMutable(uint32_t event);

    bool containsEvents(uint32_t events) const;
    bool containsAtLeastOneEvents(uint32_t events) const;
    
    bool containsEvents(uint32_t emutable, uint32_t events) const;
    bool containsAtLeastOneEvents(uint32_t emutable, uint32_t events) const;
    
    struct epoll_event epollEvents();
    struct pollfd pollEvents();
private:
    int fd_;
    state state_;
    type type_;
    uint32_t eorigin_;
    uint32_t mutable_;
};

std::ostream& operator<<(std::ostream& os, Events& events);

}
#endif // FAS_EVENTS_H