#include <ServiceManager.h>
#include <ServiceClientPool.h>

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
