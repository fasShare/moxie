#include <TcpServer.h>
#include <HandlePool.h>
#include <TcpHandle.h>
#include <Eventsops.h>
#include <Events.h>
#include <Timestamp.h>
#include <TcpConnection.h>

#include <boost/shared_ptr.hpp>

using namespace moxie;

void read_cb(boost::shared_ptr<TcpConnection> conn, Timestamp time) {
    conn->sendString(conn->getReadBuffer()->retrieveAllAsString());
    TcpConnection::shutdown(conn);
}

int main() {
    TcpServer *ser = new TcpServer(NetAddress(AF_INET, 6686, "127.0.0.1"), 10);
    auto tcphandle = new TcpHandle();
    tcphandle->setReadCallback(read_cb);

    Eventsops::RegisterEventHandler(Events::type::TCPCONN, new TcpHandle());
    Eventsops::RegisterEventHandler(Events::type::TCPSERVER, ser);
    
    ser->start();
    return 0;
}
