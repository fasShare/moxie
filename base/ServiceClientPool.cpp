#include <ServiceClientPool.h>
#include <MutexLocker.h>
#include <Log.h>

moxie::ServiceClientPool* moxie::ServiceClientPool::instance_ = nullptr; 
moxie::ServiceClientPool::ServiceClientPool() :
	mutex_(),
	services_() {
}

bool moxie::ServiceClientPool::addServiceClient(long tid, const std::string& name, boost::shared_ptr<ServiceClient> service) {
    MutexLocker locker(mutex_);
    if (service == nullptr) {
        return false;
    }
    if (services_.find(tid) == services_.end()) {
	    services_[tid] = std::map<std::string, boost::shared_ptr<ServiceClient>>();
    }
    services_[tid][name] = service;
    return true;
}

boost::shared_ptr<moxie::ServiceClient> moxie::ServiceClientPool::getService(long tid,
                                                            const std::string& name) {
    MutexLocker locker(mutex_);
    auto iter = services_.find(tid);
    if (iter == services_.end()) {
        return nullptr;
    }
    auto result = iter->second.find(name);
    if (result == iter->second.end()) {
        return nullptr;
    }
    return result->second;
}

std::vector<boost::shared_ptr<moxie::ServiceClient>> moxie::ServiceClientPool::getService(const std::string& name) {
	MutexLocker locker(mutex_);
	std::vector<boost::shared_ptr<ServiceClient>> serv;
	for (auto &service : services_) {
		for (auto &client : service.second) {
			if (client.first == name) {
				serv.emplace_back(client.second);
			}
		}
	}
	return std::move(serv);
}

moxie::ServiceClientPool *moxie::ServiceClientPool::Instance() {
	if (nullptr == instance_) {
		instance_ = new (std::nothrow) ServiceClientPool();
	}
	return instance_;
}
