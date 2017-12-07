#include <TcpConnection.h>
#include <TcpConnPool.h>
#include <EventLoopPool.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Eventsops.h>
#include <Socket.h>
#include <Thread.h>
#include <Log.h>
#include <Buffer.h>

#include <errno.h>

#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

moxie::TcpConnection::TcpConnection() {
}

bool moxie::TcpConnection::reset() {
	event_.reset();
	readBuffer_->retrieveAll();
	writeBuffer_->retrieveAll();
	shouldClose_ = false;
	tid_ = -1;
	dataEmpty_ = true;
}

bool moxie::TcpConnection::init(boost::shared_ptr<Events>& event, const NetAddress& peerAddr, Timestamp now) {
    event_.reset();
	event_ = event;
    if (nullptr == readBuffer_) {
        readBuffer_ = boost::shared_ptr<Buffer>(new (std::nothrow) Buffer(1024));
    } else {
        readBuffer_->retrieveAll();
    }
    if (nullptr == writeBuffer_) {
        writeBuffer_ = boost::shared_ptr<Buffer>(new (std::nothrow) Buffer(1024));
    } else {
        writeBuffer_->retrieveAll();
    }
    peerAddr_ = peerAddr;
    shouldClose_ = false;
    acceptTime_ = now;
    dataEmpty_ = true;
    tid_ = -1;
    return true;
}

int moxie::TcpConnection::getConnfd() const {
    return event_->getFd();
}

void moxie::TcpConnection::setPeerAddr(const moxie::NetAddress& addr) {
    peerAddr_ = addr;
}

void moxie::TcpConnection::sendString(const std::string& msg) {
    putDataToWriteBuffer(msg.c_str(), msg.size());
}

void moxie::TcpConnection::sendData(const void *data, size_t len) {
    putDataToWriteBuffer(data, len);
}

void moxie::TcpConnection::putDataToWriteBuffer(const void *data, size_t len) {
    setDataEmpty(false);
    writeBuffer_->append(data, len);
    enableWrite();
}

bool moxie::TcpConnection::shouldClose() {
    return shouldClose_;
}

void moxie::TcpConnection::setShouldClose(bool down) {
    shouldClose_ = down;
}

bool moxie::TcpConnection::shutdown(boost::shared_ptr<TcpConnection> conn) {
    if (!conn->dataEmpty()) {
        conn->setShouldClose(true);
        return true;
    }
    Eventsops::RemoveEventFromLoop(conn->event_);
    TcpConnPool::RemoveTcpConn(conn);
    return true;
}

void moxie::TcpConnection::setDataEmpty(bool empty) {
    dataEmpty_ = empty;
}

bool moxie::TcpConnection::dataEmpty() {
    return dataEmpty_;
}

bool moxie::TcpConnection::enableWrite() {
    event_->addEvent(kWriteEvent);
    return updateEvent();
}

bool moxie::TcpConnection::disableWrite() {
    event_->deleteEvent(kWriteEvent);
    return updateEvent();
}

bool moxie::TcpConnection::enableRead() {
    event_->addEvent(kReadEvent);
    return updateEvent();
}

bool moxie::TcpConnection::disableRead() {
    event_->deleteEvent(kReadEvent);
    return updateEvent();
}

bool moxie::TcpConnection::updateEvent() {
    auto loop = EventLoopPool::GetLoop(getTid());
    if (!loop) {
        assert(false);
    }
    loop->updateEvents(event_);
    return true;
}

void moxie::TcpConnection::setWriteDone(WriteDone writeDone) {
	writeDone_ = writeDone;
}
void moxie::TcpConnection::setHasData(HasData hasData) {
	hasData_ = hasData;
}
void moxie::TcpConnection::setWillBeClose(WillBeClose beClose) {
	beClose_ = beClose;
}

moxie::TcpConnection::WriteDone moxie::TcpConnection::getWriteDone() {
	return writeDone_;
}
moxie::TcpConnection::HasData moxie::TcpConnection::getHasData() {
	return hasData_;
}
moxie::TcpConnection::WillBeClose moxie::TcpConnection::getWillBeClose() {
	return beClose_;
}

boost::shared_ptr<moxie::Buffer> moxie::TcpConnection::getWriteBuffer() {
    return writeBuffer_;
}

boost::shared_ptr<moxie::Buffer> moxie::TcpConnection::getReadBuffer() {
    return readBuffer_;
}

moxie::TcpConnection::~TcpConnection() {
    LOGGER_TRACE("tid: " << gettid() << " TcpConnection destroy!");
}
