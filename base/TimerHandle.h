#ifndef MOXIE_TIMERHANDLE_H
#define MOXIE_TIMERHANDLE_H
#include <Events.h>
#include <Handler.h>

#include <boost/function.hpp>

namespace moxie {

class TimerHandle : public Handler {
public:
    void handleRead(boost::shared_ptr<Events> revents, Timestamp time);
    void handleWrite(boost::shared_ptr<Events> revents, Timestamp time);
    void handleError(boost::shared_ptr<Events> revents, Timestamp time);
    void handleClose(boost::shared_ptr<Events> revents, Timestamp time);
};

}
#endif

