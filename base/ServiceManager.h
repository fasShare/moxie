#ifndef MOXIE_SERVICEMANAGER_H
#define MOXIE_SERVICEMANAGER_H
#include <NetAddress.h>
#include <ServiceClient.h>

#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

namespace moxie {

class ServiceManager {
public:
    bool createClientForThisThread();
private:
    std::map<std::string, std::vector<NetAddress>> addrs_;
    std::map<std::string, boost::shared_ptr<ServiceClient>> services_;
};

}

#endif 
