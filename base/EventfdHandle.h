#ifndef MOXIE_EVENTFDHANDLE_H
#define MOXIE_EVENTFDHANDLE_H
#include <Events.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Timestamp.h>
#include <Buffer.h>
#include <Handler.h>
#include <TcpConnection.h>

#include <boost/function.hpp>

namespace moxie {

class EventfdHandle : public Handler {
public:
    void handleRead(boost::shared_ptr<Events> revents, Timestamp time);
    void handleWrite(boost::shared_ptr<Events> revents, Timestamp time);
    void handleError(boost::shared_ptr<Events> revents, Timestamp time);
    void handleClose(boost::shared_ptr<Events> revents, Timestamp time);
};

}
#endif // MOXIE_EVENTFDHANDLE_H

