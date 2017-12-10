#include <Poll.h>
#include <MutexLocker.h>
#include <TcpClient.h>
#include <Thread.h>
#include <Timer.h>
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
        if (!buildClientOfAddr(addr[index], clients)) {
            LOGGER_ERROR("build client of service[" << name << "] error");
            return false;
        }
        LOGGER_TRACE("build " << clients.size() << " of service[" << name << "]");
        addrs_.emplace_back(addr[index]);
        boost::shared_ptr<ClientSet> clientset(new ClientSet);
        for (size_t k = 0; k < clients.size(); ++k) {
            clients[k]->setIndex(index);
            clients[k]->setService(this);
        }
        assert(clientset->store(clients));
        free_.emplace_back(clientset);
        // put empty clientset to used.
        used_.emplace_back(new ClientSet);
        status_.emplace_back(ALIVE);
    }
    return true;
}

boost::shared_ptr<moxie::TcpClient> moxie::ServiceClient::getClient() {
    if (free_.size() != used_.size() || free_.size() == 0 || used_.size() == 0) {
        LOGGER_ERROR("check size error in getClient.");
        return nullptr;
    }
    size_t index = 0;

    auto clientset = free_[index];
    size_t size = 0;
    boost::shared_ptr<moxie::TcpClient> client;
    {
        MutexLocker locker(mutex_);
        size = clientset->size();
        // 如果有空闲client的话则直接取
        if (size > 0) {
            client = clientset->fetch();
            used_[index]->store(client);
            LOGGER_TRACE("free client num[" << clientset->size()
                    << "] used client num[" << used_[index]->size() << "].");
            return client;
        }
    }
    // 没有client则调到条件循环中等待，只等待3s，有空闲client
    int waitcount = 0;
    while (size == 0 && waitcount < 3) {
        LOGGER_TRACE("free client num[" << size << "].");
        // 开辟一个线程创建新的client,暂时使用这种方式，后续会做优化 
        Thread thread(boost::bind(&ServiceClient::buildNewClientThread, this, index));
        thread.start();
        
        cond_.waitForSeconds(1);
        waitcount++;
        size = clientset->size();
        if (size > 0) {
            client = clientset->fetch();
            used_[index]->store(client);
            LOGGER_TRACE("free client num[" << clientset->size() 
                    << "] used client num[" << used_[index]->size() << "].");
            mutex_.unlock();
            return client;
        }
    }
    return client;
}

void moxie::ServiceClient::beginCheckServerAlive(const NetAddress& addr, size_t index, const std::string& name) {
    MutexLocker locker(mutex_);
    status_[index] = CHECKING;
}

bool moxie::ServiceClient::removeClientUsed(size_t index, boost::shared_ptr<TcpClient> client) {
    if (free_.size() != used_.size() || free_.size() == 0
        || used_.size() == 0 || index >= used_.size()) {
        return false;
    }
    {
        MutexLocker locker(mutex_);
        free_[index]->store(client);
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
            LOGGER_ERROR("Socket connect error of ip[" << addr.getIp() 
                                << "] port[" << addr.getPort() << "].");
            continue;
        }
        boost::shared_ptr<moxie::Events> event(new Events(sock.getSocket(), 
                    moxie::kWriteEvent|moxie::kReadEvent));
        eventMap[sock.getSocket()] = event;
        poller.EventsAdd(event.get());
        count++;
    }
    if (count == 0) {
        LOGGER_ERROR("no avild connected socket");
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
        if (index < addrs_.size()) {
            beginCheckServerAlive(addrs_[index], index, name_);
        }
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
