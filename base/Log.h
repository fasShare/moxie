#ifndef MOXIE_LOG_H
#define MOXIE_LOG_H
#include <iostream>
#include <glog/logging.h>

namespace moxie {
#define LOGGER_TRACE(MSG) (LOG(INFO) << MSG)
#define LOGGER_DEBUG(MSG) (LOG(INFO) << MSG)
#define LOGGER_INFO(MSG) (LOG(INFO) << MSG)
#define LOGGER_WARN(MSG) (LOG(WARNING) << MSG)
#define LOGGER_ERROR(MSG) (LOG(ERROR) << MSG)
#define LOGGER_FATAL(MSG) (LOG(FATAL) << MSG)
#define LOGGER_SYSERR(MSG) (LOG(ERROR) << MSG)

#define LOG_INFO google::GLOG_INFO				//1
#define LOG_WARN google::GLOG_WARNING			//2
#define LOG_ERROR google::GLOG_ERROR			//3
#define LOG_FATAL google::GLOG_FATAL			//4

struct LogConf {
	std::string logdir;
	int minlevelout;
	std::string warnsuffix;
	std::string noticesuffix;
	int logbufsecs;
	int maxlogsize;
	bool logtostderr;
	bool alsologtostderr;

	bool init() {	
		logdir = "";
		minlevelout = LOG_WARN;
		warnsuffix = "";
		noticesuffix = "";
		logbufsecs = 0;
		maxlogsize = 10;
		logtostderr = false;
		alsologtostderr = false;
		return true;
	}
};

class CommonLog {
public:
    static bool OpenLog(const LogConf& conf) {
        return Instance()->openLog(conf);
    }
    static void CloseLog() {
        Instance()->closeLog();
    }
    
    ~CommonLog() {
        google::ShutdownGoogleLogging();
    }
private:
	CommonLog() = default;
    bool openLog(const LogConf& conf); 

    static CommonLog *Instance();

    void closeLog();
    static CommonLog *logger_;
    std::string logdir_;
	std::string noticesuffix_;
	std::string warnsuffix_;
    int outlevel_;
};

}
#endif // MOXIE_LOG_H

