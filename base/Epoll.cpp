#include <iostream>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>


#include <Epoll.h>
#include <Log.h>
#include <Timestamp.h>
#include <Events.h>

moxie::Epoll::Epoll() :
    epoll_fd_(-1),
    revents_(),
    maxNum_(20),
    addStep_(15) {//FIXME : EPOLL_CLOEXEC
    epoll_fd_ = ::epoll_create(1);
    if(epoll_fd_ == -1) {
        LOGGER_SYSERR("epoll_create error : " << ::strerror(errno));
    }
    revents_.resize(maxNum_);
}

bool moxie::Epoll::eventCtl(int op, int sd, EpollEvent* event) {
    assert(epoll_fd_ != -1);
    int ret = ::epoll_ctl(epoll_fd_, op, sd, event);
    if (ret == -1) {
        LOGGER_SYSERR("epoll_ctl error : " << ::strerror(errno) << " fd : " << sd);
        return false;
    } else {
        return true;
    }
}

bool moxie::Epoll::eventAdd(int sd, EpollEvent* event) {
    return eventCtl(EPOLL_CTL_ADD, sd, event);
}

bool moxie::Epoll::eventDel(int sd, EpollEvent* event) {
    return eventCtl(EPOLL_CTL_DEL, sd, event);
}

bool moxie::Epoll::eventMod(int sd, EpollEvent* event) {
    return eventCtl(EPOLL_CTL_MOD, sd, event);
}

int moxie::Epoll::loopWait(EpollEvent* events, int maxevents, int timeout) {
    //FIXME : The call was interrupted by a signal
should_continue:
    int ret = ::epoll_wait(epoll_fd_, events, maxevents, timeout);
    if (ret == -1) {
        if (errno == EINTR) {
            LOGGER_SYSERR("epoll_wait error : " << ::strerror(errno));
            goto should_continue;
        }
        LOGGER_SYSERR("epoll_wait error : " << ::strerror(errno));
    }
    return ret;
}

bool moxie::Epoll::EventsAdd(Events* events) {
    EpollEvent event = events->epollEvents();
    return this->eventAdd(event.data.fd, &event);
}

bool moxie::Epoll::EventsMod(Events* events) {
    EpollEvent event = events->epollEvents();
    return this->eventMod(event.data.fd, &event);
}

bool moxie::Epoll::EventsDel(Events* events) {
    EpollEvent event = events->epollEvents();
    return this->eventDel(event.data.fd, &event);
}

moxie::Timestamp moxie::Epoll::Loop(std::vector<PollerEvent_t> &events, int timeout) {
    int ret = this->loopWait(revents_.data(), maxNum_, timeout);
    for(int i = 0; i < ret; i++) {
        PollerEvent_t event;
        event.fd = revents_[i].data.fd;
        event.event = revents_[i].events;
        events.emplace_back(event);
    }
    //no use!
    revents_.clear();
    if (ret == maxNum_) {
        maxNum_ += addStep_;
        revents_.resize(maxNum_);
    }
    return Timestamp::now();
}

moxie::Epoll::~Epoll() {
    ::close(epoll_fd_);
}

