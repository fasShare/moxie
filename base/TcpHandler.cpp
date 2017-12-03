#include <TcpHandler.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <TcpConnPool.h>
#include <EventLoopPool.h>
#include <Socket.h>
#include <Log.h>
#include <Buffer.h>
#include <Thread.h>

#include <errno.h>
#include <assert.h>

#include <boost/core/ignore_unused.hpp>

void fas::TcpHandle::handleRead(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_TRACE("In TcpHandle read.");
    auto conn = TcpConnPool::Instance()->getTcpConn(gettid(), revents->getFd());
    if (!(conn && revents->getFd() == conn->getConnfd())) {
        if (conn == nullptr) {
            LOGGER_TRACE("conn is null.");
        } else {
            LOGGER_TRACE("revents->getFd() != conn->getConnfd()");
        }
        assert(false);
    }
    auto readBuffer = conn->getReadBuffer();
    int err = 0;
    ssize_t ret = readBuffer->readFd(revents->getFd(), &err);
    if (ret == 0) {
        handleClose(revents, time);
    } else if (ret < 0) {
        LOGGER_DEBUG("readBuffer_.readFd return -1 : " << ::strerror(err));
    } else if (read_cb_) {
        //transfer data to back.
        read_cb_(conn, time);
    } 
    std::cout << readBuffer->retrieveAllAsString() << std::endl;
}

void fas::TcpHandle::handleWrite(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_TRACE("In TcpHandle write.");
    auto conn = TcpConnPool::Instance()->getTcpConn(gettid(), revents->getFd());
    if (!(conn && revents->getFd() == conn->getConnfd())) {
        assert(false);
    }
    auto writeBuffer = conn->getWriteBuffer();
    int writeSd = revents->getFd();
    size_t readablesizes = writeBuffer->readableBytes();
reWrite:
    ssize_t ret = ::write(writeSd, writeBuffer->peek(), readablesizes);
    if (ret < 0) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
            //no data
            return;
        } else if (errno == EINTR) {
            goto reWrite;
        }
        LOGGER_SYSERR("TcpConnection::handleWrite error" <<  ::strerror(errno));
    } else if (ret == 0) {
        handleClose(revents, time);
    } else {
        readablesizes -= ret;
        writeBuffer->retrieve(ret);
        if (readablesizes == 0) {
            conn->setDataEmpty(true);
            conn->disableWrite();
            if (conn->shouldClose()) {
                revents->setState(Events::state::DEL);
                TcpConnPool::Instance()->removeTcpConn(gettid(), revents->getFd());
            }
            auto loop = EventLoopPool::Instance()->getLoop(gettid());
            if (!loop) {
                assert(false);
            }
            loop->updateEvents(revents);
        }
    }
    //write data out.
    if (write_cb_) {
        write_cb_(conn, time);
    }
}

void fas::TcpHandle::handleError(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_TRACE("In TcpHandle error.");
    auto conn = TcpConnPool::Instance()->getTcpConn(gettid(), revents->getFd());
    if (!(conn && revents->getFd() == conn->getConnfd())) {
        assert(false);
    }
    //remove fd from epoll
    auto loop = EventLoopPool::Instance()->getLoop(gettid());
    assert(loop != nullptr);
    revents->setState(Events::state::DEL);
    loop->updateEvents(revents);
    TcpConnPool::Instance()->removeTcpConn(gettid(), revents->getFd());
    if (error_cb_) {
        error_cb_(conn, time);
    }
}

void fas::TcpHandle::handleClose(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_TRACE("In TcpHandle close.");
    auto conn = TcpConnPool::Instance()->getTcpConn(gettid(), revents->getFd());
    if (!(conn && revents->getFd() == conn->getConnfd())) {
        assert(false);
    }
    auto loop = EventLoopPool::Instance()->getLoop(gettid());
    assert(loop != nullptr);
    revents->setState(Events::state::DEL);
    loop->updateEvents(revents);
    
    TcpConnPool::Instance()->removeTcpConn(gettid(), revents->getFd());
    if (close_cb_) {
        close_cb_(conn, time);
    }
    LOGGER_TRACE("TcpHandle close end.");
}

void fas::TcpHandle::setReadCallback(TcpConnCallback rbc) {
    read_cb_ = rbc;
}

void fas::TcpHandle::setErrorCallback(TcpConnCallback ebc) {
    error_cb_ = ebc;
}

void fas::TcpHandle::setWriteCallback(TcpConnCallback wbc) {
    write_cb_ = wbc;
}

void fas::TcpHandle::setCloseCallback(TcpConnCallback cbc) {
    close_cb_ = cbc;
}

