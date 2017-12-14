#include <MoxieConf.h>

#include <json/json.h>
#include <fstream>

int moxie::MoxieConf::getThreadNum() const {
    return threadNum_;
}

moxie::LogConf moxie::MoxieConf::getLogConf() const {
	return logconf_;	
}

const std::map<std::string, std::vector<moxie::NetAddress>>& moxie::MoxieConf::getAddress() const {
    return addrs_;
}

bool moxie::MoxieConf::load(const std::string& conf) {
    std::ifstream in;
    in.open(conf, std::ios_base::in);

    if (!in.is_open()) {
        std::cerr << "open " << conf << " error" << std::endl;
        return false;
    }
    Json::Reader jsonReader;
    Json::Value root;

    if (!jsonReader.parse(in, root)) {
        std::cerr << "parse " << conf << " error" << std::endl;
        return false;
    }

    if (root.isMember("threadnum") && root["threadnum"].isInt()) {
        threadNum_ = root["threadnum"].asInt();
    }
    if (root.isMember("logger") && root["logger"].isObject()) {
        if (!parseLogger(root["logger"])) {
            std::cerr << "parse logger error." << std::endl;
            return false;
        }
    }
    if (root.isMember("service") && root["service"].isArray()) {
        if (!parseService(root["service"])) {
            std::cerr << "parse service error." << std::endl;
            return false;
        }
    }
    return true;
}

bool moxie::MoxieConf::parseLogger(const Json::Value& root) {
    if (root.isMember("minlevelout") && root["minlevelout"].isInt()) {
        logconf_.minlevelout = root["minlevelout"].asInt();
    }
    if (root.isMember("logdir") && root["logdir"].isString()) {
        logconf_.logdir = root["logdir"].asString();
    }
	if (root.isMember("warnsuffix") && root["warnsuffix"].isString()) {
		logconf_.warnsuffix = root["warnsuffix"].asString();
	}
	if (root.isMember("noticesuffix") && root["noticesuffix"].isString()) {
		logconf_.noticesuffix = root["noticesuffix"].asString();
	}
	if (root.isMember("logbufsecs") && root["logbufsecs"].isInt()) {
		logconf_.logbufsecs = root["logbufsecs"].asInt();
	}
	if (root.isMember("maxlogsize") && root["maxlogsize"].isInt()) {
		logconf_.maxlogsize = root["maxlogsize"].asInt();
	}
	if (root.isMember("logtostderr") && root["logtostderr"].isBool()) {
		logconf_.logtostderr = root["logtostderr"].asBool();
	}
	if (root.isMember("alsologtostderr") && root["alsologtostderr"].isBool()) {
		logconf_.alsologtostderr = root["alsologtostderr"].asBool();
	}
    return true;
}

bool moxie::MoxieConf::parseServiceItem(const Json::Value& root) {
    if (!(root.isMember("name") 
        && root.isMember("address")
        && root["name"].isString()
        && root["address"].isArray())) {
        std::cerr << "name or address not find in service item" << std::endl;
        return false;
    }
    std::string name = root["name"].asString();
    std::vector<NetAddress> address;
    for (size_t k = 0; k < root["address"].size(); ++k) {
        auto item = root["address"][k];
        if (!(item.isMember("ip") 
            && item.isMember("port")
            && item["ip"].isString()
            && item["port"].isInt())) {
            std::cerr << "ip or port not found in address item." << std::endl;
            return false;
        }
        int port = item["port"].asInt();
        std::string ip = item["ip"].asString();
        NetAddress addr(AF_INET, port, ip.c_str());
        address.emplace_back(addr);
    }
    addrs_[name] = std::move(address);
    return true;
}

bool moxie::MoxieConf::parseService(const Json::Value& root) {
	std::cerr << "service length:" << root.size() << std::endl;
	for (size_t i = 0; i < root.size(); ++i) {
        parseServiceItem(root[i]);
    }
    return true;
}
