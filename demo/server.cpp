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
    conn->sendString(conn->getReadBuffer()->retrieveAllAsString());
    //TcpConnection::shutdown(conn);
}

void NewConnection(boost::shared_ptr<TcpConnection> conn, Timestamp time) {
	(void)time;
	conn->setHasData(HasData);
}

int main() {
	CommonLog::MinLogLevelOutput(LOG_WARN);
	FLAGS_logbufsecs = 0;

	MoxieArgsType args;
	args.ThreadNum = 0;

	Moxie::MoxieInit(args);
    
	TcpServer *ser = new TcpServer(NetAddress(AF_INET, 6686, "127.0.0.1"), 10);
	ser->setNewConnCallback(NewConnection);
    ser->start();

	Moxie::Run();
    return 0;
}
