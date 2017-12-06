#ifndef FAS_TCPSERVER_H
#define FAS_TCPSERVER_H
#include <memory>
#include <map>
#include <vector>
#include <queue>

#include <Socket.h>
#include <Events.h>
#include <Handler.h>
#include <NetAddress.h>
#include <TcpConnection.h>
#include <Mutex.h>
#include <MutexLocker.h>
#include <SigIgnore.h>

#include <boost/function.hpp>

namespace fas {

class TcpServer : public Handler{
public:
    using TcpConnCallback = boost::function<void (const boost::shared_ptr<TcpConnection>, Timestamp)>;
    TcpServer(const NetAddress& addr, int threadNum = 4);
    ~TcpServer();

    bool start();

    void handleRead(boost::shared_ptr<Events> revents, Timestamp time);
    void handleWrite(boost::shared_ptr<Events> revents, Timestamp time);
    void handleError(boost::shared_ptr<Events> revents, Timestamp time);
    void handleClose(boost::shared_ptr<Events> revents, Timestamp time);

    void setNewConnCallback(TcpConnCallback ncb);
private:
    Socket server_;
    boost::shared_ptr<Events> events_;
    NetAddress addr_;
    const uint listenBacklog_;
    TcpConnCallback newConnCb_;
};

}
#endif // FAS_TCPSERVER_H
