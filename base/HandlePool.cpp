#include <map>

#include <HandlePool.h>
#include <Handler.h>
#include <Events.h>

fas::HandlePool* fas::HandlePool::pool_ = nullptr;
fas::HandlePool* fas::HandlePool::Instance() {
    if (pool_ == nullptr) {
        pool_ = new HandlePool();
    }
    return pool_;
}

fas::Handler *fas::HandlePool::getHandler(Events::type type) {
    auto iter = handlers_.find(type);
    if (iter != handlers_.end()) {
        return iter->second;
    }
    return nullptr;
}

bool fas::HandlePool::addHandler(Events::type type, Handler *handler) {
    if (handler == nullptr) {
        return false;
    }
    handlers_[type] = handler;

    return true;
}
