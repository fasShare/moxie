#ifndef MOXIE_TCPCLIENTPOOL_H
#define MOXIE_TCPCLIENTPOOL_H
#include <unordered_multimap>
#include <string>
#include <Mutex.h>
#include <TcpCLient.h>
#include <boost/shared_ptr.hpp>

namespace moxie {

class TcpClientPool {
public:
	boost::shared_ptr<TcpClient> getClient(const std::string& service);
	bool putClient(boost::shared_ptr<TcpClient> client);
private:
	Mutex mutex_;
	std::unordered_multimap<std::string, boost::shared_ptr<TcpClient>> free_;
	std::unordered_multimap<std::string, boost::shared_ptr<TcpClient>> used_;
};

}

#endif //MOXIE_TCPCLIENTPOOL_H
