#ifndef MOXIE_SERVICECLIEBTPOOL_H
#define MOXIE_SERVICECLIEBTPOOL_H
#include <map>
#include <vector>
#include <unistd.h>
#include <atomic>

#include <ServiceClient.h>
#include <Mutex.h>

namespace moxie {

class ServiceClientPool {
public:
    static bool AddServiceClient(const std::string& name, 
            boost::shared_ptr<ServiceClient> service) {
        return Instance()->addServiceClient(service->getTid(), name, service);
    }
    static boost::shared_ptr<ServiceClient> GetService(long tid,
                                            const std::string& name) {
        return Instance()->getService(tid, name);
    }
	static std::vector<boost::shared_ptr<ServiceClient>> GetService(const std::string& name) {
		return Instance()->getService(name);
	}
private:
    bool addServiceClient(long tid, const std::string& name,
                        boost::shared_ptr<ServiceClient> service);
    boost::shared_ptr<ServiceClient> getService(long tid, const std::string& name);
	std::vector<boost::shared_ptr<ServiceClient>> getService(const std::string& name);
    static ServiceClientPool *Instance();
    ServiceClientPool();
    
    Mutex mutex_;
    std::map<long, std::map<std::string, boost::shared_ptr<ServiceClient>>> services_;
    static ServiceClientPool *instance_;
};

}

#endif //MOXIE_SERVICECLIEBTPOOL_H
