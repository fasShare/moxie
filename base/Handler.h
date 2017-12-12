#ifndef MOXIE_HANDLER_H
#define MOXIE_HANDLER_H
#include <Events.h>
#include <Timestamp.h>
#include <MoxieCoroutine.h>
#include <CoroutinePool.h>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

namespace moxie {

class Handler {
public:
    virtual ~Handler() {};
    void doHandle(boost::shared_ptr<Events> events, Timestamp time) {
        boost::shared_ptr<MoxieCoroutine> co(new MoxieCoroutine(boost::bind(&Handler::coroutine, this, events, time)));
        CoroutinePool::PutMoxieCoroutine(events, co);
        co->run();
    }
    void coroutine(boost::shared_ptr<Events> events, Timestamp time) {
        if (events->isClose()) {
            handleClose(events, time);
        } else if (events->isError()) {
            handleError(events, time);
        } else if (events->isRead()) {
            handleRead(events, time);
        } else if (events->isWrite()) {
            handleWrite(events, time);
        }   
        CoroutinePool::RemoveCoroutine(events);
    }
    virtual void handleRead(boost::shared_ptr<Events> revents, Timestamp time) = 0;
    virtual void handleWrite(boost::shared_ptr<Events> revents, Timestamp time) = 0;
    virtual void handleError(boost::shared_ptr<Events> revents, Timestamp time) = 0;
    virtual void handleClose(boost::shared_ptr<Events> revents, Timestamp time) = 0;
};

}
#endif // MOXIE_HANDLER_H

