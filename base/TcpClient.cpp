#include <TcpClient.h>
#include <Socket.h>
#include <TcpConnPool.h>
#include <Eventsops.h>
#include <EventLoopPool.h>
#include <ServiceClient.h>
#include <Thread.h>
#include <Log.h>

moxie::TcpClient::TcpClient(const NetAddress& addr) :
	response(),
	request(),
	conn_(new TcpConnection()),
	transfer_(nullptr),
    addr_(addr),
    sock_(AF_INET, SOCK_STREAM, 0),
    index_(-1),
    service_(nullptr) {
}

bool moxie::TcpClient::connectToServer() {
    sock_.setExecClose();
    if (sock_.connect(addr_)) {
        boost::shared_ptr<Events> event(new Events(sock_.getSocket(), kNoneEvent));
    	sock_.setNoBlocking();
        conn_->init(event, addr_, Timestamp::now());
        return true;
    }
    return false;
}

bool moxie::TcpClient::initWithEvent(boost::shared_ptr<Events> event) {
    conn_->init(event, addr_, Timestamp::now());
    return true;
}

void moxie::TcpClient::setService(ServiceClient *service) {
    service_ = service;
}

void moxie::TcpClient::HasData(boost::shared_ptr<TcpConnection> conn, Timestamp time,
								boost::shared_ptr<TcpClient> client) {
	assert(conn->getConnfd() == client->conn_->getConnfd());
	auto transfer = client->transfer_;
	size_t length = 0;
	switch (transfer->DataCheck(conn, length)) {
		case DataTransfer::DATA_OK:
			if (transfer->DataFetch(conn, length, client->request, client->response)) {
				client->done(client->request, client->response);
                RecycleClient(client);
			} else {
				LOGGER_WARN("DataFetch failed!");
			}
			break;
		case DataTransfer::DATA_INCOMPLETE:
			break;
		case DataTransfer::DATA_ERROR:
            client->done(client->request, client->response);
            TcpConnection::shutdown(conn);
            RecycleClient(client);
		default:
			assert(false);
	}
}

void moxie::TcpClient::setIndex(long index) {
    index_ = index;
}

long moxie::TcpClient::getIndex() const {
    return index_;
}

bool moxie::TcpClient::RecycleClient(boost::shared_ptr<TcpClient> client) {
    assert(client->service_);
    assert(client->index_ != -1);
    auto service = client->service_;
    return service->removeClientUsed(client->index_, client);
}

void moxie::TcpClient::WriteDone(boost::shared_ptr<TcpConnection> conn, Timestamp time, 
								boost::shared_ptr<TcpClient> client) {
	assert(conn->getConnfd() == client->conn_->getConnfd());
	conn->getEvent()->updateEvents(kReadEvent);
	Eventsops::UpdateLoopEvents(conn->getEvent());
}
void moxie::TcpClient::WillBeClose(boost::shared_ptr<TcpConnection> conn, Timestamp time,
									boost::shared_ptr<TcpClient> client) {
	assert(conn->getConnfd() == client->conn_->getConnfd());
	LOGGER_ERROR("Client will be close.");
}

void moxie::TcpClient::SetDataTransfer(DataTransfer *transfer) {
	if (transfer_ == nullptr) {
		transfer_ = transfer;
	}
}
bool moxie::TcpClient::Talk(boost::shared_ptr<TcpClient> client, 
							boost::shared_ptr<Message> request,
							boost::shared_ptr<Message> response, TalkDone done) {
    if (!done) {
        LOGGER_WARN("TcpClient's TalkDone isn't seted.");
        return false;
    }
    if (client->transfer_ == nullptr) {
		LOGGER_WARN("TcpClient data transfer is not seted.");
		done(request, response);
		return false;
	}
	if (request->length() == 0) {
		LOGGER_WARN("request is empty.");
		done(request, response);
		return false;
	}
	
	auto writebuffer = client->conn_->getWriteBuffer();
	if (writebuffer->readableBytes() > 0) {
		LOGGER_WARN("write buffer isn't empty.");
		writebuffer->retrieveAll();
	}
	auto loop = EventLoopPool::GetLoop(gettid());
	writebuffer->append(request->data(), request->length());

    client->request = request;
    client->response = response;
    client->done = done;

    client->conn_->setTid(loop->getTid());
    client->conn_->getEvent()->setTid(loop->getTid());

	client->conn_->setWriteDone(boost::bind(&TcpClient::WriteDone, _1, _2, client));
	client->conn_->setHasData(boost::bind(&TcpClient::HasData, _1, _2, client));
	client->conn_->setWillBeClose(boost::bind(&TcpClient::WillBeClose, _1, _2, client));
	TcpConnPool::AddTcpConn(client->conn_);

	client->conn_->getEvent()->updateEvents(kWriteEvent);
    client->conn_->getEvent()->setType(EVENT_TYPE_TCPCON);
	loop->updateEvents(client->conn_->getEvent());
	
	return true;
}
