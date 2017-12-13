#include <Log.h>

using namespace moxie;

int main() {
	LogConf conf;
    CommonLog::OpenLog(conf);

    LOGGER_INFO("warning test.");

    CommonLog::CloseLog();

    LOGGER_ERROR("fatal test.");

    return 0;
}
