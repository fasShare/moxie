#include <iostream>
#include <memory>
#include <unistd.h>
#include <new>

#include <Socket.h>
#include <Timestamp.h>
#include <Log.h>
#include <EventLoop.h>
#include <TcpServer.h>
#include <HandlePool.h>
#include <TcpConnPool.h>
#include <EventLoopPool.h>

#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

moxie::TcpServer::TcpServer(const NetAddress& addr, int threadNum) :
    loop_(nullptr),
    server_(AF_INET, SOCK_STREAM, 0),
    events_(new Events(server_.getSocket(), kReadEvent)),
    addr_(addr),
    listenBacklog_(50) {
    assert(server_.setNoBlocking());
    assert(server_.setExecClose());
    assert(server_.bind(addr_));
    assert(server_.listen(listenBacklog_));
    events_->setType(EVENT_TYPE_TCPSER);
	LOGGER_TRACE("server listen fd = " << server_.getSocket());
}

bool moxie::TcpServer::start() { 
	loop_ = EventLoopPool::GetMainLoop();
	if (!loop_) {
		LOGGER_ERROR("Please check MoxieInit() was called!");
		return false;
	}	
    loop_->updateEvents(events_);
	HandlePool::AddHandler(EVENT_TYPE_TCPSER, this);
    return true;
}

void moxie::TcpServer::handleRead(boost::shared_ptr<Events> event, Timestamp time) {
    LOGGER_TRACE("In TcpServer hand read.");
    loop_->assertInOwnerThread();
    boost::ignore_unused(time);

    moxie::EventLoop *workloop = nullptr;

    if (event->getFd() == server_.getSocket()) {
        moxie::NetAddress peerAddr;
        Timestamp acceptTime = Timestamp::now();
        int sd = server_.accept(peerAddr, true);
        if (sd < 0) {
            LOGGER_SYSERR("In Tcpserver accepted return an error!");
            return;
        }
        workloop = EventLoopPool::GetNextLoop();
        if (nullptr == workloop) {
            workloop = loop_;
        }
        long looptid = workloop->getTid();

        boost::shared_ptr<Events> conn_event(new moxie::Events(sd, kReadEvent));
        conn_event->setType(EVENT_TYPE_TCPCON);
        conn_event->setTid(looptid);
        
        moxie::TcpConnection::TcpConnShreadPtr sconn(new moxie::TcpConnection());
        sconn->init(conn_event, peerAddr, acceptTime);
        
        sconn->setTid(looptid);
        TcpConnPool::AddTcpConn(sconn);
        workloop->updateEvents(conn_event);
        if (newConnCb_) {
            newConnCb_(sconn, acceptTime);
        }
    } else {
        LOGGER_ERROR("event.getFd() == server_.getSocket()");
    }
}

void moxie::TcpServer::handleWrite(boost::shared_ptr<Events> revents, Timestamp time) {
	boost::ignore_unused(revents, time);
	LOGGER_ERROR("In handle write of TcpServer.");
}

void moxie::TcpServer::handleError(boost::shared_ptr<Events> revents, Timestamp time) {
	boost::ignore_unused(revents, time);
	LOGGER_ERROR("In handle error of TcpServer.");
}

void moxie::TcpServer::handleClose(boost::shared_ptr<Events> revents, Timestamp time) {
	boost::ignore_unused(revents, time);
	LOGGER_ERROR("In handle close of TcpServer.");
}

void moxie::TcpServer::setNewConnCallback(TcpConnCallback ncb) {
    newConnCb_ = ncb;
}

moxie::TcpServer::~TcpServer() {
    loop_->quit();
    LOGGER_TRACE("TcpServer will be destroyed in process " << getpid());
}
