#include <TcpClient.h>
#include <TcpConnPool.h>
#include <EventLoopPool.h>
#include <Log.h>

moxie::TcpClient::TcpClient(const NetAddress& addr) :
	response(),
	request(),
	conn_(new TcpConnection()),
	transfer_(nullptr) {
}

void moxie::TcpClient::HasData(boost::shared_ptr<TcpConnection> conn, Timestamp time,
								boost::shared_ptr<TcpClient> client) {
	assert(conn->getConnfd() == client->conn_->getConnfd());
	auto transfer = client->transfer_;
	int length = 0;
	switch (transfer->DataCheck(conn, length)) {
		case DataTransfer::DATA_ERROR:
		case DataTransfer::DATA_OK:
			if (transfer->DataFetch(conn, length, client->request, client->response)) {
				client->done(client->request, client->response);
			} else {
				LOGGER_WARN("DataFetch failed!");
			}
			break;
		case DataTransfer::DATA_INCOMPLETE:
			break;
		case DataTransfer::DATA_FATAL:
		default:
			assert(false);
	}
}
void moxie::TcpClient::WriteDone(boost::shared_ptr<TcpConnection> conn, Timestamp time, 
								boost::shared_ptr<TcpClient> client) {
	assert(conn->getConnfd() == client->conn_->getConnfd());
	conn->disableWrite();
	conn->enableRead();
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
	writebuffer->append(request->data(), request->length());
	
	client->conn_->setWriteDone(boost::bind(&TcpClient::WriteDone, _1, _2, client));
	client->conn_->setHasData(boost::bind(&TcpClient::HasData, _1, _2, client));
	client->conn_->setWillBeClose(boost::bind(&TcpClient::WillBeClose, _1, _2, client));
	TcpConnPool::AddTcpConn(client->conn_);
	
	auto loop = EventLoopPool::GetNextLoop();
	client->conn_->enableWrite();
	client->conn_->disableRead();
	loop->updateEvents(client->conn_->getEvent());
	
	return true;
}
