#ifndef MOXIE_MEMCACHED_H
#define MOXIE_MEMCACHED_H
#include <TcpClient.h>

namespace moxie {

class MemCached : public DataTransfer {
public:
    RET DataCheck(boost::shared_ptr<TcpConnection> conn, size_t& length);
    bool DataFetch(boost::shared_ptr<TcpConnection> conn,
                    size_t length_from_check,
                    boost::shared_ptr<Message> request,
                    boost::shared_ptr<Message> response);
};

}

#endif // MOXIE_MEMCACHED_H
