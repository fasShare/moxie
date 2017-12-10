#include <HandlePool.h>
#include <TcpHandle.h>
#include <Eventsops.h>
#include <Events.h>
#include <Timestamp.h>
#include <TcpConnection.h>
#include <Moxie.h>
#include <MoxieConf.h>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <Message.h>
#include <Timerops.h>
#include <TcpClient.h>
#include <ServiceClientPool.h>

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

void Timerfunc() {
	auto service = ServiceClientPool::GetService(gettid(), "Server");

    if (service == nullptr) {
        LOGGER_WARN("Get nullptr service.");
        return;
    }
    LOGGER_TRACE("Before getClient.");
    auto client = service->getClient();
    LOGGER_TRACE("After getClient.");
    client->SetDataTransfer(new mydataTransfer());

    boost::shared_ptr<Message> request(new Message(1024)), response(new Message(1024));
	request->append("shangxiaofei", strlen("shangxiaofei"));
	TcpClient::Talk(client, request, response, TalkDone);
}

int main() {
    MoxieConf conf;
    conf.load("./conf/MoxieClient.conf");

	Moxie::MoxieInit(conf);
    
	Timer *timer = new Timer(Timerfunc, addTime(Timestamp::now(), 5), 5);
	Timerops::AddTimer(timer);

	Moxie::Run();
    return 0;
}
