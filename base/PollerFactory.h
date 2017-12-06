#ifndef MOXIE_POLLERFACTORY_H
#define MOXIE_POLLERFACTORY_H
#include <Poller.h>
#include <VirtualBase.h>

namespace moxie {

class PollerFactory : public VirtualBase {
public:
    virtual Poller *getPoller() = 0;
};

}

#endif //MOXIE_POLLERFACTORY_H
