#ifndef MOXIE_MEMSERVER_H
#define MOXIE_MEMSERVER_H
#include <unordered_map>
#include <boost/shared_ptr.hpp>

#include <TcpServer.h>
#include <TcpConnection.h>

namespace moxie {

class MemServer {
public:
    MemServer();
    ~MemServer();
private:
    void willBeClose(boost::shared_ptr<TcpConnection> conn, Timestamp time);
    void newConn(boost::shared_ptr<TcpConnection> conn, Timestamp time);
    
    TcpServer server_;
    std::unordered_map<int, shared_ptr<MemBroker>> brokers_;
};

}

#endif //MOXIE_MEMSERVER_H
