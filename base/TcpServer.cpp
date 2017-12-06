#include <iostream>
#include <memory>
#include <unistd.h>
#include <new>

#include <Socket.h>
#include <Timestamp.h>
#include <Log.h>
#include <EventLoop.h>
#include <TcpServer.h>
#include <TcpConnPool.h>
#include <EventLoopPool.h>

#include <boost/bind.hpp>
#include <boost/core/ignore_unused.hpp>

fas::TcpServer::TcpServer(const NetAddress& addr, int threadNum) :
    signor_(),
    server_(AF_INET, SOCK_STREAM, 0),
    loop_(nullptr),
    events_(new Events(server_.getSocket(), kReadEvent)),
    addr_(addr),
    listenBacklog_(50) {
    server_.setNoBlocking();
    server_.setExecClose();
    server_.bind(addr_);
    server_.listen(listenBacklog_);
    events_->setType(EVENT_TYPE_TCPSER);
	LOGGER_TRACE("server listen fd = " << server_.getSocket());
}

bool fas::TcpServer::start() { 
	loop_ = EventLoopPool::GetMainLoop();
	if (!loop_) {
		LOGGER_ERROR("Please check MoxieInit() was called!");
		return false;
	}	
    loop_->updateEvents(events_);
	HandlePool::AddHandler(EVENT_TYPE_TCPSER, this);
    return true;
}

void fas::TcpServer::handleRead(boost::shared_ptr<Events> event, Timestamp time) {
    LOGGER_TRACE("In TcpServer hand read.");
    loop_->assertInOwnerThread();
    boost::ignore_unused(time);

    fas::EventLoop *workloop = nullptr;

    if (event->getFd() == server_.getSocket()) {
        fas::NetAddress peerAddr;
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

        boost::shared_ptr<Events> conn_event(new fas::Events(sd, kReadEvent));
        conn_event->setType(EVENT_TYPE_TCPCON);
        conn_event->setTid(looptid);
        
        fas::TcpConnection::TcpConnShreadPtr sconn(new fas::TcpConnection());
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

void fas::TcpServer::handleWrite(boost::shared_ptr<Events> revents, Timestamp time) {
	boost::ignore_unused(events, time);
	LOGGER_ERROR("In handle write of TcpServer.");
}

void fas::TcpServer::handleError(boost::shared_ptr<Events> revents, Timestamp time) {
	boost::ignore_unused(events, time);
	LOGGER_ERROR("In handle error of TcpServer.");
}

void fas::TcpServer::handleClose(boost::shared_ptr<Events> revents, Timestamp time) {
	boost::ignore_unused(events, time);
	LOGGER_ERROR("In handle close of TcpServer.");
}

void fas::TcpServer::setNewConnCallback(TcpConnCallback ncb) {
    newConnCb_ = ncb;
}

fas::TcpServer::~TcpServer() {
    loop_->quit();
    LOGGER_TRACE("TcpServer will be destroyed in process " << getpid());
}
