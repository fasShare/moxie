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
    bool store(std::vector<boost::shared_ptr<TcpClient>>& clients) {
        for (auto& client : clients) {
            if (!clients_.emplace(client).second) {
                return false;
            }
        }
        return true;
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
    enum status {
        ALIVE,
        DEAD,
        CHECKING,
    };
	enum LoadBalance {
		ROBIN,
	};
    ServiceClient();
    bool init(const std::vector<NetAddress>& addr, const std::string& name);
	boost::shared_ptr<TcpClient> getClient(const std::string& key, LoadBalance py);
	boost::shared_ptr<TcpClient> getClient();
    bool putClient(boost::shared_ptr<TcpClient>);
    void buildNewClientThread(size_t index);
    bool removeClientUsed(size_t index, boost::shared_ptr<TcpClient> client);
	boost::shared_ptr<moxie::TcpClient> fetchClientFromOther(boost::shared_ptr<ServiceClient> service, size_t index);

    void setTid(long tid);
    long getTid() const;
    void setName(const std::string& name);
    std::string getName() const;
	size_t getFreeSize();

	bool checkInOwner() const;
private:
	boost::shared_ptr<TcpClient> removeClientToOther(size_t index);
	size_t getBackEnd(const std::string& key, LoadBalance py);
	boost::shared_ptr<TcpClient> getClient(size_t index);
	boost::shared_ptr<TcpClient> removeClient(size_t index);
    bool buildClientOfAddr(const NetAddress& addr, std::vector<boost::shared_ptr<TcpClient>>& clients);
	boost::shared_ptr<moxie::TcpClient> getClientGlobal(const std::string& name_, const size_t index);
	bool checkConnectSucc(int sd);
    void beginCheckServerAlive(const NetAddress& addr, size_t index, const std::string& name);
    long tid_;
	size_t freeSize_;
    Mutex mutex_;
    Condition cond_;
    std::string name_;
    std::vector<NetAddress> addrs_;
    std::vector<boost::shared_ptr<ClientSet>> free_;
    std::vector<boost::shared_ptr<ClientSet>> used_;
    std::vector<status> status_;
};

}

#endif //MOXIE_SERVICECLIENT_H
