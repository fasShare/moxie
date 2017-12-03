#ifndef FAS_EVENTFDHANDLE_H
#define FAS_EVENTFDHANDLE_H
#include <Events.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Timestamp.h>
#include <Buffer.h>
#include <Handler.h>
#include <TcpConnection.h>

#include <boost/function.hpp>

namespace fas {

class EventfdHandle : public Handler {
public:
    void handleRead(boost::shared_ptr<Events> revents, Timestamp time);
    void handleWrite(boost::shared_ptr<Events> revents, Timestamp time);
    void handleError(boost::shared_ptr<Events> revents, Timestamp time);
    void handleClose(boost::shared_ptr<Events> revents, Timestamp time);
};

}
#endif // FAS_EVENTFDHANDLE_H

