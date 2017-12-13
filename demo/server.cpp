#include <TcpServer.h>
#include <HandlePool.h>
#include <TcpHandle.h>
#include <Eventsops.h>
#include <Events.h>
#include <Timestamp.h>
#include <TcpConnection.h>
#include <Moxie.h>
#include <boost/shared_ptr.hpp>

using namespace moxie;

void HasData(boost::shared_ptr<TcpConnection> conn, Timestamp time) {
	std::string recv = conn->getReadBuffer()->retrieveAllAsString();
	std::cout << "recv:" << recv << std::endl;
    conn->sendString(recv);
    //TcpConnection::shutdown(conn);
}

void NewConnection(boost::shared_ptr<TcpConnection> conn, Timestamp time) {
	(void)time;
	conn->setHasData(HasData);
}

int main() {
    MoxieConf conf;
    conf.load("./conf/moxie.conf");

	Moxie::MoxieInit(conf);
    
	TcpServer *ser = new TcpServer(NetAddress(AF_INET, 6686, "127.0.0.1"), 10);
	ser->setNewConnCallback(NewConnection);
    ser->start();

	Moxie::Run();
    return 0;
}
