#include <EventfdHandle.h>
#include <Events.h>
#include <NetAddress.h>
#include <Timestamp.h>

void moxie::EventfdHandle::handleRead(boost::shared_ptr<Events> revents, Timestamp time) {
    uint64_t one = 1;
    ssize_t n = ::read(revents->getFd(), &one, sizeof one);
    if (n != sizeof one){
        LOGGER_ERROR("EventLoop::handleRead() reads " << n << " bytes instead of 8");
    }
}
void moxie::EventfdHandle::handleWrite(boost::shared_ptr<Events> revents, Timestamp time) {

}
void moxie::EventfdHandle::handleError(boost::shared_ptr<Events> revents, Timestamp time) {

}
void moxie::EventfdHandle::handleClose(boost::shared_ptr<Events> revents, Timestamp time) {

}

