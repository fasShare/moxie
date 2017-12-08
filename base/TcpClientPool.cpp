#include <unordered_multimap>
#include <string>
#include <TcpCLient.h>
#include <MutexLocker.h>
#include <boost/shared_ptr.hpp>

boost::shared_ptr<TcpClient> TcpClientPool::getClient(const std::string& service) {
	auto iter = free_.find(service);
	if (iter == free_.end()) {
		if (!buildNewClientOfService(service, free_)) {
			return nullptr;
		}
		iter = free_.find(service);
		if (iter == free_.end()) {
			return nullptr;
		}
	}
	return iter->second;
}

bool TcpClientPool::putClient(boost::shared_ptr<TcpClient> client) {
	free_[client->getService()] = client;
}

