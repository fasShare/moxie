#ifndef MOXIE_MOXIECONF_H
#define MOXIE_MOXIECONF_H
#include <NetAddress.h>
#include <Log.h>

#include <json/json.h>
#include <vector>
#include <map>

namespace moxie {

class MoxieConf {
public:
    bool load(const std::string& conf);
    int getThreadNum() const;
    const std::map<std::string, std::vector<NetAddress>>& getAddress() const;
	LogConf getLogConf() const;
private:
    bool parseLogger(const Json::Value& root);
    bool parseService(const Json::Value& root);
    bool parseServiceItem(const Json::Value& root);

	LogConf logconf_;
    std::string logdir_;
    int minleveloutstd_;
    int threadNum_;
    std::vector<std::string> service_;
    std::map<std::string, std::vector<NetAddress>> addrs_;
};

}

#endif //MOXIE_MOXIECONF_H
