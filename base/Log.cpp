#include <new>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

#include <Log.h>

fas::CommonLog* fas::CommonLog::logger_ = nullptr;

fas::CommonLog* fas::CommonLog::Instance() {
    if (NULL == logger_) {
        logger_ = new(std::nothrow) CommonLog();
    }
    return logger_;
}

bool fas::CommonLog::openLog(std::string logdir, int level, std::string warn, std::string error, std::string info, std::string fatal) {
    google::InitGoogleLogging("");

    logdir_ == "" ? "./" : logdir_;
    if (::access(logdir.c_str(), F_OK|W_OK|R_OK)) {
        std::cerr << "fas has no permission of log dir "<< logdir  << std::endl;
        return false;
    }

    warn = warn == "" ? "warning" : warn;
    error = error == "" ? "error" : error;
    fatal = fatal == "" ? error : fatal;
    info = info == "" ? warn : info;

    google::SetLogDestination(google::GLOG_FATAL, fatal.c_str());
    google::SetLogDestination(google::GLOG_ERROR, error.c_str());
    google::SetLogDestination(google::GLOG_WARNING, warn.c_str());
    google::SetLogDestination(google::GLOG_INFO, info.c_str());
    
    FLAGS_logbufsecs = 1;
    FLAGS_max_log_size = 10;
    FLAGS_stop_logging_if_full_disk = true;
    google::SetLogFilenameExtension(".log");

    return true;
}

void fas::CommonLog::closeLog() {
    google::ShutdownGoogleLogging();
}

