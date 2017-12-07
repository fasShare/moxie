#ifndef MOXIE_EPOLLFACTORY_H
#define MOXIE_EPOLLFACTORY_H
#include <new>

#include <Epoll.h>
#include <PollerFactory.h>

namespace moxie {

class EpollFactory : public PollerFactory {
public:
    Epoll* getPoller() {
        return new (std::nothrow) Epoll();
    }
};

}

#endif //MOXIE_EPOLLFACTORY_H
