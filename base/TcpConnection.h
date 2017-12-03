#ifndef FAS_TCPCONNECTION_H
#define FAS_TCPCONNECTION_H
#include <Events.h>
#include <Socket.h>
#include <NetAddress.h>
#include <Timestamp.h>
#include <Buffer.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

namespace fas {

class buffer;

class TcpConnection {
public:
using TcpConnShreadPtr = boost::shared_ptr<TcpConnection>;
using TcpConnectionPtr = TcpConnection*;
using TcpConnMessageCallback = boost::function<void (TcpConnShreadPtr,
                                boost::shared_ptr<Buffer>, Timestamp)>;
using CloseCallback = boost::function<void ()>;
    TcpConnection();
    ~TcpConnection();

    bool init(boost::shared_ptr<Events>& event, const NetAddress& peerAddr, Timestamp now);

    int getConnfd() const;
    void setPeerAddr(const NetAddress& addr);
    
    void setTid(long tid) { tid_ = tid; };
    long getTid() { return tid_; };
    
    void sendString(const std::string& msg);
    void sendData(const void *data, size_t len);
    void putDataToWriteBuffer(const void *data, size_t len);

    bool shouldClose();
    void setShouldClose(bool down);
    bool shutdown();

    void setDataEmpty(bool empty);
    bool dataEmpty();

    bool enableWrite();
    bool disableWrite();
    bool enableRead();
    bool disableRead();

    boost::shared_ptr<Buffer> getReadBuffer();
    boost::shared_ptr<Buffer> getWriteBuffer();
private:
    bool updateEvent();
private:
    boost::shared_ptr<Events> event_;
    boost::shared_ptr<Buffer> readBuffer_;
    boost::shared_ptr<Buffer> writeBuffer_;
    int connfd_;
    NetAddress peerAddr_;
    bool shouldClose_;
    Timestamp acceptTime_;
    bool dataEmpty_;
    long tid_;
};

}
#endif // FAS_TCPCONNECTION_H

