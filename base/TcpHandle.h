#ifndef FAS_TCPHANDLE_H
#define FAS_TCPHANDLE_H
#include <Events.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Timestamp.h>
#include <Buffer.h>
#include <Handler.h>
#include <TcpConnection.h>

#include <boost/function.hpp>

namespace fas {

class TcpHandle : public Handler {
public:
    using TcpConnShreadPtr = boost::shared_ptr<TcpConnection>;
    using TcpConnCallback =  boost::function<void (TcpConnShreadPtr, Timestamp)>;
    void handleRead(boost::shared_ptr<Events> revents, Timestamp time);
    void handleWrite(boost::shared_ptr<Events> revents, Timestamp time);
    void handleError(boost::shared_ptr<Events> revents, Timestamp time);
    void handleClose(boost::shared_ptr<Events> revents, Timestamp time);

    void setReadCallback(TcpConnCallback rbc);
    void setErrorCallback(TcpConnCallback ebc);
    void setWriteCallback(TcpConnCallback wbc);
    void setCloseCallback(TcpConnCallback cbc);
private:
    TcpConnCallback read_cb_;
    TcpConnCallback write_cb_;
    TcpConnCallback error_cb_;
    TcpConnCallback close_cb_;
};

}
#endif // FAS_TCPCONNECTION_H

