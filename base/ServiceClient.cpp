#include <Poll.h>
#include <MutexLocker.h>
#include <TcpClient.h>
#include <Thread.h>
#include <ServiceClient.h>

#include <unordered_map>
#include <vector>
#include <list>

moxie::ServiceClient::ServiceClient() :
    mutex_(), 
    cond_(mutex_),
    name_(""),
    addrs_(),
    free_(),
    used_() {
}

bool moxie::ServiceClient::init(const std::vector<NetAddress>& addr, const std::string& name) {
    name_ = name;
    for (auto iter = addr.begin(); iter != addr.end(); ++iter) {
        boost::shared_ptr<TcpClient> client(new TcpClient(*iter));
        if (!client->connectToServer()) {
            return false;
        }
        addrs_.emplace_back(*iter);
        boost::shared_ptr<ClientSet> clientset(new ClientSet);
        assert(clientset->store(client));
        free_.emplace_back(clientset);
    }
    return true;
}

boost::shared_ptr<moxie::TcpClient> moxie::ServiceClient::getClient() {
    if (free_.size() != used_.size() || free_.size() == 0 || used_.size() == 0) {
        return nullptr;
    }
    size_t index = 0;

    auto clientset = free_[index];
    size_t size = 0;
    {
        MutexLocker locker(mutex_);
        size = clientset->size();
    }
    while (size == 0) {
        cond_.wait();
        size = clientset->size();
        Thread thread(boost::bind(&ServiceClient::buildNewClientThread, this, index));
        thread.start();
    }
    auto client = clientset->fetch();
    used_[index]->store(client);
    return client;
}

bool moxie::ServiceClient::putClient(boost::shared_ptr<TcpClient> client) {
    if (free_.size() != used_.size() || free_.size() == 0 || used_.size() == 0) {
        return false;
    }
    size_t index = 0;
    {
        MutexLocker locker(mutex_);
        free_[index]->store(client);
        assert(used_[index]->erase(client));
    }
    cond_.notifyAll();
    return true;
}

void  moxie::ServiceClient::buildNewClientThread(size_t index) {
    std::unordered_map<int, boost::shared_ptr<moxie::Events>> eventMap;
    int interval = 5;
    int count = 0;
    moxie::Poll poller;
    for (int i = 0; i < interval; ++i) {
        moxie::Socket sock(AF_INET, SOCK_STREAM, 0);
        sock.setNoBlocking();
        sock.setExecClose();
        if (!sock.connect(addrs_[index])) {
            continue;
        }
        boost::shared_ptr<moxie::Events> event(new Events(sock.getSocket(), 
                    moxie::kWriteEvent|moxie::kReadEvent));
        eventMap[sock.getSocket()] = event;
        poller.EventsAdd(event.get());
        count++;
    }

    int timeout = 100;
    std::vector<PollerEvent_t> revents;
    poller.Loop(revents, timeout);
    if (revents.size() == 0) {
        return;
    }

    std::vector<boost::shared_ptr<TcpClient>> clients;
    for (size_t i = 0; i < revents.size(); ++i) {
        if (!checkConnectSucc(revents[i].fd)) {
            continue;
        }
        boost::shared_ptr<moxie::Events> event = eventMap[revents[i].fd];
        boost::shared_ptr<TcpClient> client(new TcpClient(addrs_[index]));
        client->initWithEvent(event);
        clients.emplace_back(client);
    }
    if (clients.size() > 0) {
        MutexLocker locker(mutex_);
        for (size_t k = 0; k < clients.size(); ++k) {
            free_[index]->store(clients[k]);
        }
    }
    cond_.notifyAll();
}

bool moxie::ServiceClient::checkConnectSucc(int sd) {
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(sd, SOL_SOCKET, SO_ERROR,
                reinterpret_cast<char *>(&error), &len) == 0) {
        if (error != 0) {
            LOGGER_ERROR(" getsockopt : " << ::strerror(error));
            return false;
        }
        return true;
    } else {
        LOGGER_ERROR(" getsockopt : " << ::strerror(errno));
        return false;
    } 
}
