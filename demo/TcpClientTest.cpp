#include <HandlePool.h>
#include <TcpHandle.h>
#include <Eventsops.h>
#include <Events.h>
#include <Timestamp.h>
#include <TcpConnection.h>
#include <Moxie.h>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <Message.h>
#include <Timerops.h>
#include <TcpClient.h>

using namespace moxie;

class mydataTransfer : public DataTransfer {
public:
    RET DataCheck(boost::shared_ptr<TcpConnection> conn, size_t& length) {
        length = conn->getReadBuffer()->readableBytes();
        return DataTransfer::DATA_OK;
    }
    bool DataFetch(boost::shared_ptr<TcpConnection> conn, size_t length_from_check,
            boost::shared_ptr<Message> request, boost::shared_ptr<Message> response) {
        response->append(conn->getReadBuffer()->peek(), length_from_check);
		conn->getReadBuffer()->retrieve(length_from_check);
        return true;
    }
};

void TalkDone(boost::shared_ptr<Message> request,
            boost::shared_ptr<Message> response) {
    std::cout << response->data() << std::endl;
}

void Timerfunc(boost::shared_ptr<TcpClient> client) {
	boost::shared_ptr<Message> request(new Message(1024)), response(new Message(1024));
	request->append("shangxiaofei", strlen("shangxiaofei"));
	TcpClient::Talk(client, request, response, TalkDone);
}

int main() {
	MoxieArgsType args;
	args.ThreadNum = 0;

	Moxie::MoxieInit(args);

    NetAddress addr(AF_INET, 6686, "127.0.0.1");
    boost::shared_ptr<TcpClient> client(new TcpClient(addr));
    if (!client->connectToServer()) {
        std::cerr << "connect to server error." << std::endl;
        return -1;
    }
    client->SetDataTransfer(new mydataTransfer());
    
	Timer *timer = new Timer(boost::bind(Timerfunc, client), addTime(Timestamp::now(), 5), 3);
	Timerops::AddTimer(timer);

	Moxie::Run();
    return 0;
}
