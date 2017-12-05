#include <Log.h>

using namespace fas;

int main() {
    CommonLog::OpenLog("./", LOG_WARN, "warning", "error", "info", "fatal");

    LOGGER_INFO("warning test.");

    CommonLog::CloseLog();

    LOGGER_ERROR("fatal test.");

    return 0;
}
