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
#include <ThreadPool.h>
#include <TcpConnection.h>
#include <Mutex.h>
#include <MutexLocker.h>
#include <SigIgnore.h>

#include <boost/function.hpp>

namespace fas {

class EventLoop;

class TcpServer : public Handler{
public:
    using TcpConnCallback = boost::function<void (const boost::shared_ptr<Events>, Timestamp)>;
    TcpServer(const NetAddress& addr, int threadNum = 4);
    ~TcpServer();

    EventLoop* getLoop() const;

	void setLoop(fas::EventLoop *loop);

    bool start();

    static void LoopThreadFunc();

    void handleRead(boost::shared_ptr<Events> revents, Timestamp time);
    void handleWrite(boost::shared_ptr<Events> revents, Timestamp time);
    void handleError(boost::shared_ptr<Events> revents, Timestamp time);
    void handleClose(boost::shared_ptr<Events> revents, Timestamp time);

    void setNewConnCallback(TcpConnCallback ncb);
private:
    SigIgnore signor_;
    Socket server_;
    EventLoop *loop_;
    int threadNum_;
    boost::shared_ptr<Events> events_;
    NetAddress addr_;
    const uint listenBacklog_;
    ThreadPool *threadPool_;
    TcpConnCallback newConnCb_;
};

}
#endif // FAS_TCPSERVER_H
