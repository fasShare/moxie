#include <iostream>
#include <memory>
#include <unistd.h>
#include <new>


#include <Socket.h>
#include <Timestamp.h>
#include <Handle.h>
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
    loop_(new EventLoop),
    threadNum_(threadNum <= 0 ? 1: threadNum),
    events_(new Events(server_.getSocket(), kReadEvent)),
    addr_(addr),
    listenBacklog_(50) {
    server_.setNoBlocking();
    server_.setExecClose();
    server_.bind(addr_);
    server_.listen(listenBacklog_);
    events_->setType(Events::type::TCP_SERVER);
	LOGGER_TRACE("server listen fd = " << server_.getSocket());
    loop_->updateEvents(events_);
    EventLoopPool::Instance()->addEventLoop(gettid(), loop_, true);
}

fas::EventLoop* fas::TcpServer::getLoop() const{
    return loop_;
}

void fas::TcpServer::setLoop(fas::EventLoop *loop) {
	loop_ = loop;
}

bool fas::TcpServer::start() {  
	if (!loop_) {
		LOGGER_ERROR("TcpServer's loop is nullptr.");
		return false;
	}
    threadPool_ = new (std::nothrow) ThreadPool(threadNum_, TcpServer::LoopThreadFunc, "ThreadPool");
    assert(threadPool_);
    threadPool_->start();
    loop_->loop();
    return true;
}

void fas::TcpServer::handleRead(boost::shared_ptr<Events> event, Timestamp time) {
    LOGGER_TRACE("In TcpServer hand read.");
    loop_->assertInOwnerThread();
    boost::ignore_unused(time);

    fas::EventLoop *workloop = loop_;

    if (event->getFd() == server_.getSocket()) {
        fas::NetAddress peerAddr;
        Timestamp acceptTime = Timestamp::now();
        int sd = server_.accept(peerAddr, true);
        if (sd < 0) {
            LOGGER_SYSERR("In Tcpserver accepted return an error!");
            return;
        }
        workloop = EventLoopPool::Instance()->getNextLoop();
        if (nullptr == workloop) {
            workloop = loop_;
        }

        boost::shared_ptr<Events> conn_event(new fas::Events(sd, kReadEvent));
        conn_event->setType(Events::type::TCP);
        
        fas::TcpConnection::TcpConnShreadPtr sconn(new fas::TcpConnection());
        sconn->init(conn_event, peerAddr, acceptTime);
        
        fas::TcpConnPool *pool = TcpConnPool::Instance();
        long looptid = workloop->getTid();
        sconn->setTid(looptid);
        pool->addTcpConn(looptid, sd, sconn);
        workloop->updateEvents(conn_event);
        if (newConnCb_) {
            newConnCb_(conn_event, acceptTime);
        }
    } else {
        LOGGER_ERROR("event.getFd() == server_.getSocket()");
    }
}

void fas::TcpServer::handleWrite(boost::shared_ptr<Events> revents, Timestamp time) {
    LOGGER_ERROR("In handle write of TcpServer.");
}

void fas::TcpServer::handleError(boost::shared_ptr<Events> revents, Timestamp time) {
    LOGGER_ERROR("In handle error of TcpServer.");
}

void fas::TcpServer::handleClose(boost::shared_ptr<Events> revents, Timestamp time) {
    LOGGER_ERROR("In handle close of TcpServer.");
}

void fas::TcpServer::LoopThreadFunc() {
    EventLoop *loop = new (std::nothrow) EventLoop();
    if (nullptr == loop) {
        return;
    }

    if (!EventLoopPool::Instance()->addEventLoop(gettid(), loop)) {
        return;
    }

    loop->loop();
}

void fas::TcpServer::setNewConnCallback(TcpConnCallback ncb) {
    newConnCb_ = ncb;
}

fas::TcpServer::~TcpServer() {
    LOGGER_TRACE("TcpServer will be destroyed in process " << getpid());
}
