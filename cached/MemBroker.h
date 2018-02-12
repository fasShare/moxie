#ifndef MOXIE_MEMBROKER_H
#define MOXIE_MEMBROKER_H

namespace moxie {

class MemBroker {
public:
    void writeDone(TcpConnShreadPtr conn, Timestamp time);
    void hasData(TcpConnShreadPtr conn, Timestamp time);
    void talkDone(boost::shared_ptr<Message>, boost::shared_ptr<Message>);
private:
    bool checkDataOk(TcpConnShreadPtr conn);
    std::string key_;
};

}

#endif //MOXIE_MEMBROKER_H
