#ifndef MOXIE_COROUTINEPOOL_H
#define MOXIE_COROUTINEPOOL_H
#include <MoxieCoroutine.h>
#include <Events.h>
#include <Mutex.h>
#include <MutexLocker.h>

#include <map>
#include <boost/shared_ptr.hpp>

namespace moxie {

class CoroutinePool {
public:
    static boost::shared_ptr<MoxieCoroutine> GetMoxieCoroutine(boost::shared_ptr<Events> event) {
        return Instance()->getMoxieCoroutine(event);
    }
    static bool PutMoxieCoroutine(boost::shared_ptr<Events> event,
            boost::shared_ptr<MoxieCoroutine> con) {
        return Instance()->putMoxieCoroutine(event, con);
    }
    static bool RemoveCoroutine(boost::shared_ptr<Events> event) {
        return Instance()->removeCoroutine(event);
    }
private:
    bool removeCoroutine(boost::shared_ptr<Events> event) {
        MutexLocker lock(mutex_);
        auto tid = event->getTid();
        auto miter = coroutines_.find(tid);
        if (miter == coroutines_.end()) {
            return false;
        }
        auto fd = event->getFd();
        auto citer = miter->second.find(fd);
        if (citer == miter->second.end()) {
            return false;
        }

        return miter->second.erase(fd) == 1;
    }
    boost::shared_ptr<MoxieCoroutine> getMoxieCoroutine(boost::shared_ptr<Events> event) {
        MutexLocker lock(mutex_);
        auto tid = event->getTid();
        auto miter = coroutines_.find(tid);
        if (miter == coroutines_.end()) {
            return nullptr;
        }
        auto fd = event->getFd();
        auto citer = miter->second.find(fd);
        if (citer == miter->second.end()) {
            return nullptr;
        }
        return miter->second[fd];
    }
    bool putMoxieCoroutine(boost::shared_ptr<Events> event, boost::shared_ptr<MoxieCoroutine> con) {
        MutexLocker lock(mutex_);
        auto tid = event->getTid();
        auto miter = coroutines_.find(tid);
        if (miter == coroutines_.end()) {
            coroutines_[tid] = std::map<int, boost::shared_ptr<MoxieCoroutine>>();
        }
        auto fd = event->getFd();
        coroutines_[tid][fd] = con;
        return true;
    }
    static CoroutinePool *Instance() {
        if (pool_ == nullptr) {
            pool_ = new CoroutinePool();
        }
        return pool_;
    }
    
    Mutex mutex_;
    static CoroutinePool *pool_;
    std::map<long, std::map<int, boost::shared_ptr<MoxieCoroutine>>> coroutines_;
};

}

#endif
