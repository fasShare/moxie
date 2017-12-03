#include <TcpConnection.h>
#include <TcpConnPool.h>
#include <EventLoopPool.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <Socket.h>
#include <Thread.h>
#include <Log.h>
#include <Buffer.h>

#include <errno.h>

#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

fas::TcpConnection::TcpConnection() {
}

bool fas::TcpConnection::init(boost::shared_ptr<Events>& event, const NetAddress& peerAddr, Timestamp now) {
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
    connfd_ = event_->getFd();
    peerAddr_ = peerAddr;
    shouldClose_ = false;
    acceptTime_ = now;
    dataEmpty_ = true;
    tid_ = -1;
    return true;
}

int fas::TcpConnection::getConnfd() const {
    return connfd_.getSocket();
}

void fas::TcpConnection::setPeerAddr(const fas::NetAddress& addr) {
    peerAddr_ = addr;
}

void fas::TcpConnection::sendString(const std::string& msg) {
    putDataToWriteBuffer(msg.c_str(), msg.size());
}

void fas::TcpConnection::sendData(const void *data, size_t len) {
    putDataToWriteBuffer(data, len);
}

void fas::TcpConnection::putDataToWriteBuffer(const void *data, size_t len) {
    setDataEmpty(false);
    writeBuffer_->append(data, len);
    enableWrite();
}

bool fas::TcpConnection::shouldClose() {
    return shouldClose_;
}

void fas::TcpConnection::setShouldClose(bool down) {
    shouldClose_ = down;
}

bool fas::TcpConnection::shutdown() {
    if (!dataEmpty()) {
        setShouldClose(true);
        return true;
    }
    auto loop = EventLoopPool::Instance()->getLoop(getTid());
    event_->setState(Events::state::DEL);
    loop->updateEvents(event_);
    TcpConnPool::Instance()->removeTcpConn(getTid(), event_->getFd());
    return true;
}

void fas::TcpConnection::setDataEmpty(bool empty) {
    dataEmpty_ = empty;
}
bool fas::TcpConnection::dataEmpty() {
    return dataEmpty_;
}

bool fas::TcpConnection::enableWrite() {
    event_->addEvent(kWriteEvent);
    return updateEvent();
}

bool fas::TcpConnection::disableWrite() {
    event_->deleteEvent(kWriteEvent);
    return updateEvent();
}

bool fas::TcpConnection::enableRead() {
    event_->addEvent(kReadEvent);
    return updateEvent();
}

bool fas::TcpConnection::disableRead() {
    event_->deleteEvent(kReadEvent);
    return updateEvent();
}

bool fas::TcpConnection::updateEvent() {
    auto loop = EventLoopPool::Instance()->getLoop(getTid());
    if (!loop) {
        assert(false);
    }
    loop->updateEvents(event_);
    return true;
}

boost::shared_ptr<fas::Buffer> fas::TcpConnection::getWriteBuffer() {
    return writeBuffer_;
}

boost::shared_ptr<fas::Buffer> fas::TcpConnection::getReadBuffer() {
    return readBuffer_;
}

fas::TcpConnection::~TcpConnection() {
    LOGGER_TRACE("tid: " << gettid() << " TcpConnection destroy!");
}
