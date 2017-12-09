#include <Poll.h>
#include <MutexLocker.h>
#include <TcpClient.h>
#include <Thread.h>
#include <ServiceClient.h>

#include <unordered_map>
#include <vector>
#include <list>

moxie::ServiceClient::ServiceClient() :
    tid_(gettid()),
    mutex_(), 
    cond_(mutex_),
    name_(""),
    addrs_(),
    free_(),
    used_() {
}

void moxie::ServiceClient::setName(const std::string& name) {
    name_ = name;
}

std::string moxie::ServiceClient::getName() const {
    return name_;
}

long moxie::ServiceClient::getTid() const {
    return tid_;
}

void moxie::ServiceClient::setTid(long tid) {
    tid_ = tid;
}

bool moxie::ServiceClient::init(const std::vector<NetAddress>& addr, const std::string& name) {
    name_ = name;
    for (size_t index  = 0; index < addr.size(); ++index) {
        std::vector<boost::shared_ptr<TcpClient>> clients;
        if (buildClientOfAddr(addr[index], clients)) {
            return false;
        }
        addrs_.emplace_back(addr[index]);
        boost::shared_ptr<ClientSet> clientset(new ClientSet);
        for (size_t k = 0; k < clients.size(); ++k) {
            clients[k]->setIndex(index);
            clients[k]->setService(this);
        }
        assert(clientset->store(clients));
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

bool moxie::ServiceClient::removeClientUsed(size_t index, boost::shared_ptr<TcpClient> client) {
    if (free_.size() != used_.size() || free_.size() == 0
        || used_.size() == 0 || index >= used_.size()) {
        return false;
    }
    {
        MutexLocker locker(mutex_);
        assert(used_[index]->erase(client));
    }
    return true;
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

bool moxie::ServiceClient::buildClientOfAddr(const NetAddress& addr, std::vector<boost::shared_ptr<TcpClient>>& clients) {
    std::unordered_map<int, boost::shared_ptr<moxie::Events>> eventMap;
    int interval = 5;
    int count = 0;
    moxie::Poll poller;
    for (int i = 0; i < interval; ++i) {
        moxie::Socket sock(AF_INET, SOCK_STREAM, 0);
        sock.setNoBlocking();
        sock.setExecClose();
        if (!sock.connect(addr)) {
            continue;
        }
        boost::shared_ptr<moxie::Events> event(new Events(sock.getSocket(), 
                    moxie::kWriteEvent|moxie::kReadEvent));
        eventMap[sock.getSocket()] = event;
        poller.EventsAdd(event.get());
        count++;
    }
    if (count == 0) {
        return false;
    }
    int timeout = 100;
    std::vector<PollerEvent_t> revents;
    poller.Loop(revents, timeout);
    if (revents.size() == 0) {
        return false;
    }
    int clientNum = 0;
    for (size_t i = 0; i < revents.size(); ++i) {
        if (!checkConnectSucc(revents[i].fd)) {
            continue;
        }
        boost::shared_ptr<moxie::Events> event = eventMap[revents[i].fd];
        boost::shared_ptr<TcpClient> client(new TcpClient(addr));
        client->initWithEvent(event);
        clients.emplace_back(client);
        clientNum++;
    }
    return clientNum > 0;
}

void moxie::ServiceClient::buildNewClientThread(size_t index) {
    std::vector<boost::shared_ptr<TcpClient>> clients;
    if (!((index < addrs_.size()) && buildClientOfAddr(addrs_[index], clients))) {
        return;
    }
    if (clients.size() > 0) {
        MutexLocker locker(mutex_);
        for (size_t k = 0; k < clients.size(); ++k) {
            clients[k]->setIndex(index);
            clients[k]->setService(this);
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
