#ifndef FAS_TCPCONNPOOL_H
#define FAS_TCPCONNPOOL_H
#include <map>
#include <sys/types.h>

#include <TcpConnection.h>
#include <Mutex.h>

#include <boost/shared_ptr.hpp>

namespace fas {
    
class TcpConnPool {
using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;
using TcpConnShreadPtr = TcpConnection::TcpConnShreadPtr;
public:
    static bool AddTcpConn(boost::shared_ptr<TcpConnection> conn) {
        return Instance()->addTcpConn(conn->getTid(), conn->getConnfd(), conn);
    }

    static TcpConnShreadPtr GetTcpConn(long tid, int fd) {
        return Instance()->getTcpConn(tid, fd);
    }

    static void RemoveTcpConn(boost::shared_ptr<TcpConnection> conn) {
        Instance()->removeTcpConn(conn->getTid(), conn->getConnfd());
    }
private:
    static TcpConnPool *Instance();
    bool addTcpConn(long tid, int fd, boost::shared_ptr<TcpConnection>& conn);
    boost::shared_ptr<TcpConnection> getTcpConn(long tid, int fd);
    void removeTcpConn(long tid, int fd);
    TcpConnPool();
    
    static TcpConnPool *instance_;
    Mutex mutex_;
    std::map<long, std::map<int, boost::shared_ptr<TcpConnection>>> conns_; 
};

}

#endif //FAS_TCPCONNPOOL_H
