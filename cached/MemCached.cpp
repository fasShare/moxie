#include <MemCached.h>

using moxie::MemCached;

RET MemCached::DataCheck(boost::shared_ptr<TcpConnection> conn, size_t& length) {
    
    return DATA_OK;
}

bool DataFetch(boost::shared_ptr<TcpConnection> conn,
                size_t length_from_check,
                boost::shared_ptr<Message> request,
                boost::shared_ptr<Message> response) {

    return true;
}

