#include <TcpServer.h>
#include <HandlePool.h>
#include <TcpHandler.h>
#include <Events.h>
#include <Timestamp.h>
#include <TcpConnection.h>

#include <boost/shared_ptr.hpp>

using namespace fas;

void read_cb(boost::shared_ptr<TcpConnection> conn, Timestamp time) {
    conn->sendString(conn->getReadBuffer()->retrieveAllAsString());
    conn->shutdown();
}

int main() {
    TcpServer *ser = new TcpServer(NetAddress(AF_INET, 6686, "127.0.0.1"), 10);
    auto tcphandle = new TcpHandle();
    tcphandle->setReadCallback(read_cb);
    HandlePool::Instance()->addHandler(Events::type::TCP, tcphandle);
    HandlePool::Instance()->addHandler(Events::type::TCP_SERVER, ser);
    ser->start();
    return 0;
}
