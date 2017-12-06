#include <TimerHandle.h>
#include <EventLoop.h>
#include <Timestamp.h>
#include <EventLoopPool.h>
#include <Log.h>

#include <errno.h>
#include <assert.h>

#include <boost/core/ignore_unused.hpp>

void fas::TimerHandle::handleRead(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_TRACE("TimerHandle read.");
	auto loop = EventLoopPool::GetLoop(revents->getTid());
	if (!loop) {
		LOGGER_ERROR("In TimerHandle get null loop.");
		return;
	}
	auto schedule = loop->getTimerSchedule();
	if (!schedule) {
		LOGGER_ERROR("In TimerHandle get null TimerSchedule.");
		return;
	}
	schedule->handleRead(revents, time);
}

void fas::TimerHandle::handleWrite(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_TRACE("TimerHandle write was invoked.");
	boost::ignore_unused(revents, time);
}

void fas::TimerHandle::handleError(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_TRACE("TimerHandle error was invoked.");
	boost::ignore_unused(revents, time);
}

void fas::TimerHandle::handleClose(boost::shared_ptr<Events> revents, fas::Timestamp time) {
    LOGGER_WARN("TimerHandle close was invoked.");
	boost::ignore_unused(revents, time);
}