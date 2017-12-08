#ifndef MOXIE_EVENTSOPS_H
#define MOXIE_EVENTSOPS_H
#include <EventLoop.h>
#include <Events.h>
#include <Handler.h>
#include <HandlePool.h>
#include <EventLoopPool.h>

#include <boost/shared_ptr.hpp>

namespace moxie {

class Eventsops {
public:
    static bool BuildEventIntoLoop(EventLoop *loop, Events* event, int type) {
        if (!loop) {
			assert(false);
            return false;
        }
        event->setType(type);
        loop->updateEvents(boost::shared_ptr<Events>(event));
        return true;
    }

    static bool RemoveEventFromLoop(boost::shared_ptr<Events> event) {
        auto loop = EventLoopPool::GetLoop(event->getTid());
        if (!loop) {
			assert(false);
            return false;
        }
        event->setState(Events::state::DEL);
        return loop->updateEvents(event);
    }

    static bool RegisterEventHandler(int type, Handler *handler) {
        return HandlePool::AddHandler(type, handler);
    }

    static Handler* EventHandler(int type) {
        return HandlePool::GetHandler(type);
    }

	static bool UpdateLoopEvents(boost::shared_ptr<Events> event) {
		auto loop = EventLoopPool::GetLoop(event->getTid());
		if (!loop) {
			assert(false);
			return false;
		}
		return loop->updateEvents(event);
	}
};

}

#endif //MOXIE_EVENTSOPS_H
