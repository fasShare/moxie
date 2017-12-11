#include <ServiceManager.h>
#include <ServiceClientPool.h>

moxie::ServiceManager::ServiceManager(std::map<std::string, std::vector<NetAddress>> addrs) :
    addrs_(addrs),
    services_() {
}

bool moxie::ServiceManager::createClientForThisThread() {
    for (auto iter = addrs_.begin(); iter != addrs_.end(); ++iter) {
        boost::shared_ptr<ServiceClient> service(new ServiceClient());
        if (!service->init(iter->second, iter->first)) {
            return false;
        }
        services_[iter->first] = service;
        ServiceClientPool::AddServiceClient(iter->first, service);
    }
    return true;
}
