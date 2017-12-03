#include <assert.h>

#include <TcpConnPool.h>
#include <MutexLocker.h>
#include <EventLoop.h>
#include <Log.h>

#include <boost/bind.hpp>

fas::TcpConnPool *fas::TcpConnPool::instance_ = nullptr;

fas::TcpConnPool::TcpConnPool() :
    mutex_() {
}

fas::TcpConnPool *fas::TcpConnPool::Instance() {
    if (nullptr == instance_) {
        instance_ = new (std::nothrow) TcpConnPool();
    }
    return instance_;
}

bool fas::TcpConnPool::addTcpConn(long tid, int key, boost::shared_ptr<TcpConnection>& conn) {
    MutexLocker locker(mutex_);
    if (conns_.end() == conns_.find(tid)) {
        conns_[tid] = std::map<int, boost::shared_ptr<TcpConnection>>(); 
    }
    if (conns_[tid].end() != conns_[tid].find(key)) {
        return false;
    }
    conns_[tid][key] = conn;
    return true;
}

boost::shared_ptr<fas::TcpConnection> fas::TcpConnPool::getTcpConn(long tid, int fd) {
    MutexLocker locker(mutex_);
    auto pair = conns_.find(tid);
    if (conns_.end() != pair) {
        auto conn = conns_[tid].find(fd);
        if (conn == conns_[tid].end()) {
            return nullptr;
        }
        return conn->second;
    }
    return nullptr;
}

void fas::TcpConnPool::removeTcpConn(long tid, int key) {
    MutexLocker locker(mutex_);
	auto iter = conns_.find(tid);
    assert(conns_.end() != iter);
	auto connmap = conns_[tid].find(key);
	assert(conns_[tid].end() != connmap);
    size_t n = conns_[tid].erase(key);
    assert(n == 1);
    LOGGER_ERROR("A TcpConnection was removed from bucket.");
}

