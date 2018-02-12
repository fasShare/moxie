#include <MemBroker.h>

void MemBroker::writeDone(TcpConnShreadPtr conn, Timestamp time) {
    
}

void MemBroker::hasData(TcpConnShreadPtr conn, Timestamp time) {
    if (checkDataOk(conn)) {
        
    }
}

void MemBroker::talkDone(boost::shared_ptr<Message>, boost::shared_ptr<Message>) {
    
}

bool MemBroker::checkDataOk(TcpConnShreadPtr conn) {
    return true;        
}
