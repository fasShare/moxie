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

#define LOG_INFO google::GLOG_INFO
#define LOG_ERROR google::GLOG_ERROR
#define LOG_FATAL google::GLOG_FATAL
#define LOG_WARN google::GLOG_WARNING

class CommonLog {
public:
    static bool OpenLog(std::string logdir, int level, std::string warn, std::string error, std::string info = "", std::string fatal = "") {
        return Instance()->openLog(logdir, level, warn, error, info, fatal);
    }
    static void CloseLog() {
        Instance()->closeLog();
    }
    static void AllLogToStderr(bool open) {
        FLAGS_logtostderr = open;
    }
    static void OutputLogToStderrAndFile(bool yesno) {
        FLAGS_alsologtostderr = yesno;
    }
    static bool MinLevelTStderrWithColor(int level, bool color) {
        if (level != LOG_INFO
            && level != LOG_ERROR
            && level != LOG_FATAL
            && level != LOG_WARN) {
            return false;
        }
        FLAGS_stderrthreshold = level;
        FLAGS_colorlogtostderr = color;
        return true; 
    }
    static bool MinLogLevelOutput(int level) {
        if (level != LOG_INFO
            && level != LOG_ERROR
            && level != LOG_FATAL
            && level != LOG_WARN) {
            return false;
        }
        FLAGS_minloglevel = level;
        return true;
    }
    
    ~CommonLog() {
        google::ShutdownGoogleLogging();
    }
private:
    CommonLog() {
		google::InitGoogleLogging("");
	}
    bool openLog(std::string logdir, int level, std::string warn, std::string error, std::string info, std::string fatal); 

    static CommonLog *Instance();

    void closeLog();
    static CommonLog *logger_;
    std::string logdir_;
    std::string warn_;
    std::string error_;
    std::string fatal_;
    std::string info_;
    int outlevel_;
};

}
#endif // MOXIE_LOG_H

