#include <TcpHandle.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <TcpConnPool.h>
#include <EventLoopPool.h>
#include <Eventsops.h>
#include <Socket.h>
#include <Log.h>
#include <Buffer.h>
#include <Thread.h>

#include <errno.h>
#include <assert.h>

#include <boost/core/ignore_unused.hpp>

void moxie::TcpHandle::handleRead(boost::shared_ptr<Events> revents, moxie::Timestamp time) {
    LOGGER_TRACE("In TcpHandle read.");
    auto conn = TcpConnPool::GetTcpConn(gettid(), revents->getFd());
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
    } else {
		auto hasdata = conn->getHasData();
        //transfer data to back.
		if (hasdata) {
        	hasdata(conn, time);
		} else {
			LOGGER_WARN("The HasData callback was not seted.");
		}
    } 
}

void moxie::TcpHandle::handleWrite(boost::shared_ptr<Events> revents, moxie::Timestamp time) {
    LOGGER_TRACE("In TcpHandle write.");
    auto conn = TcpConnPool::GetTcpConn(revents->getTid(), revents->getFd());
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
                TcpConnPool::RemoveTcpConn(conn);
            }
            auto loop = EventLoopPool::GetLoop(gettid());
            if (!loop) {
                assert(false);
            }
            loop->updateEvents(revents);
        }
    }
    //notify data write out.
	auto writedone = conn->getWriteDone();
    if (writedone) {
        writedone(conn, time);
    }
}

void moxie::TcpHandle::handleError(boost::shared_ptr<Events> revents, moxie::Timestamp time) {
    LOGGER_TRACE("TcpHandle error start");
    auto conn = TcpConnPool::GetTcpConn(revents->getTid(), revents->getFd());
    if (!(conn && revents->getFd() == conn->getConnfd())) {
        assert(false);
    }
    
    Eventsops::RemoveEventFromLoop(revents);
    TcpConnPool::RemoveTcpConn(conn);
	auto willbeclose = conn->getWillBeClose();
    if (willbeclose) {
        willbeclose(conn, time);
    }
	LOGGER_TRACE("TcpHandle error end");
}

void moxie::TcpHandle::handleClose(boost::shared_ptr<Events> revents, moxie::Timestamp time) {
    LOGGER_TRACE("TcpHandle close start");
    auto conn = TcpConnPool::GetTcpConn(revents->getTid(), revents->getFd());
    if (!(conn && revents->getFd() == conn->getConnfd())) {
        assert(false);
    }
    
    Eventsops::RemoveEventFromLoop(revents);
    TcpConnPool::RemoveTcpConn(conn);
	auto willbeclose = conn->getWillBeClose();
    if (willbeclose) {
        willbeclose(conn, time);
    }
    LOGGER_TRACE("TcpHandle close end.");
}

void moxie::TcpHandle::setReadCallback(TcpConnCallback rbc) {
    read_cb_ = rbc;
}

void moxie::TcpHandle::setErrorCallback(TcpConnCallback ebc) {
    error_cb_ = ebc;
}

void moxie::TcpHandle::setWriteCallback(TcpConnCallback wbc) {
    write_cb_ = wbc;
}

void moxie::TcpHandle::setCloseCallback(TcpConnCallback cbc) {
    close_cb_ = cbc;
}

