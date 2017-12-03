#ifndef FAS_HANDLER_H
#define FAS_HANDLER_H
#include <Events.h>
#include <Timestamp.h>

#include <boost/shared_ptr.hpp>

namespace fas {

class Handler {
public:
    virtual ~Handler() {};
    void doHandle(boost::shared_ptr<Events> events, Timestamp time) {
        if (events->isClose()) {
            handleClose(events, time);
        } else if (events->isError()) {
            handleError(events, time);
        } else if (events->isRead()) {
            handleRead(events, time);
        } else if (events->isWrite()) {
            handleWrite(events, time);
        }   

    }
    virtual void handleRead(boost::shared_ptr<Events> revents, Timestamp time) = 0;
    virtual void handleWrite(boost::shared_ptr<Events> revents, Timestamp time) = 0;
    virtual void handleError(boost::shared_ptr<Events> revents, Timestamp time) = 0;
    virtual void handleClose(boost::shared_ptr<Events> revents, Timestamp time) = 0;
};

}
#endif // FAS_HANDLER_H

