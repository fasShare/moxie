#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <TcpServer.h>
#include <TcpConnection.h>

using moxie::MemServer;

MemServer::MemServer() {
    server_.setNewConnCallback(boost::bind(&TcpServer::newConn, this, _1, _2));
}

MemServer::~MemServer() {

}

void MemServer::willBeClose(boost::shared_ptr<TcpConnection> conn, Timestamp time) {
    assert(brokers_.count(conn->getConnfd()) == 1);
    brokers_.erase(conn->getConnfd());
}

void MemServer::newConn(boost::shared_ptr<TcpConnection> conn, Timestamp time) {
    auto broker = boost::make_shared<MemBroker>();
    
    conn->setWriteDone(boost::bind(&MemBroker::writeDone, broker, _1, _2));
    conn->setHasData(boost::bind(&MemBroker::hasData, broker, _1, _2));
    conn->setWillBeClose(boost::bind(&MemServer::willBeClose, broker, _1, _2));

    assert(brokers_.count(conn->getConnfd()) == 0);

    brokers_[getConnfd()] = broker;
}

