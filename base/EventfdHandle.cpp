#include <EventfdHandle.h>
#include <Events.h>
#include <NetAddress.h>
#include <Timestamp.h>

void fas::EventfdHandle::handleRead(boost::shared_ptr<Events> revents, Timestamp time) {
    uint64_t one = 1;
    ssize_t n = ::read(revents->getFd(), &one, sizeof one);
    if (n != sizeof one){
        LOGGER_ERROR("EventLoop::handleRead() reads " << n << " bytes instead of 8");
    }
}
void fas::EventfdHandle::handleWrite(boost::shared_ptr<Events> revents, Timestamp time) {

}
void fas::EventfdHandle::handleError(boost::shared_ptr<Events> revents, Timestamp time) {

}
void fas::EventfdHandle::handleClose(boost::shared_ptr<Events> revents, Timestamp time) {

}

