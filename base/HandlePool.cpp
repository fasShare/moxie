#include <map>

#include <HandlePool.h>
#include <Handler.h>
#include <Events.h>

moxie::HandlePool* moxie::HandlePool::pool_ = nullptr;
moxie::HandlePool* moxie::HandlePool::Instance() {
    if (pool_ == nullptr) {
        pool_ = new HandlePool();
    }
    return pool_;
}

moxie::Handler *moxie::HandlePool::getHandler(int type) {
    auto iter = handlers_.find(type);
    if (iter != handlers_.end()) {
        return iter->second;
    }
    return nullptr;
}

bool moxie::HandlePool::addHandler(int type, Handler *handler) {
    if (handler == nullptr) {
        return false;
    }
    handlers_[type] = handler;

    return true;
}
