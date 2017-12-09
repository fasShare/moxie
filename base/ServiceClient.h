#ifndef MOXIE_SERVICECLIENT_H
#define MOXIE_SERVICECLIENT_H
#include <vector>
#include <Mutex.h>
#include <assert.h>
#include <string>
#include <EventLoop.h>
#include <Condition.h>
#include <boost/shared_ptr.hpp>
#include <TcpClient.h>

namespace moxie {

class ClientSet {
public:
    boost::shared_ptr<TcpClient> fetch() {
        if (clients_.size() == 0) {
            return nullptr;
        }
        auto iter = clients_.begin();
        auto client = *iter;
        assert(clients_.erase(client) == 1);
        return client;
    }
    bool store(boost::shared_ptr<TcpClient> client) {
        return clients_.emplace(client).second;
    }
    bool erase(boost::shared_ptr<TcpClient> client) {
        return clients_.erase(client) == 1;
    }
    size_t size() {
        return clients_.size();
    }
private:
    std::set<boost::shared_ptr<TcpClient>> clients_;
};

class ServiceClient {
public:
    ServiceClient();
    bool init(const std::vector<NetAddress>& addr, const std::string& name);
    boost::shared_ptr<TcpClient> getClient();
    bool putClient(boost::shared_ptr<TcpClient>);
    void buildNewClientThread(size_t index);
private:
    bool checkConnectSucc(int sd);
    Mutex mutex_;
    Condition cond_;
    std::string name_;
    std::vector<NetAddress> addrs_;
    std::vector<boost::shared_ptr<ClientSet>> free_;
    std::vector<boost::shared_ptr<ClientSet>> used_;
};

}

#endif //MOXIE_SERVICECLIENT_H
