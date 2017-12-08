#ifndef MOXIE_TIMEROPS_H
#define MOXIE_TIMEROPS_H
#include <Timer.h>
#include <EventLoopPool.h>
#include <Thread.h>

namespace moxie {

class Timerops {
public:
	static bool AddTimer(Timer *timer) {
		if (!timer) {
			return false;
		}
		auto loop = EventLoopPool::GetLoop(gettid());		
		if (!loop) {
			LOGGER_WARN("Get null EventLoop.");
			return false;
		}
		timer->setTid(loop->getTid());
		auto schedule = loop->getTimerSchedule();
		if (!schedule) {
			LOGGER_WARN("Get null TimerSchedule.");
			return false;
		}
		return schedule->addTimer(timer);
	}

	static bool DeleteTimer(Timer *timer) {
		if (!timer) {
			return false;
		}
		auto loop = EventLoopPool::GetLoop(timer->getTid());
		if (!loop) {
			LOGGER_WARN("Get null EventLoop.");
			return false;
		}
		auto schedule = loop->getTimerSchedule();
		if (!schedule) {
			LOGGER_WARN("Get null TimerSchedule.");
			return false;
		}
		schedule->delTimer(timer);
		return true;
	}
};

}

#endif //MOXIE_TIMEROPS_H
