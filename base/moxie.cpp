#include <assert.h>
#include <new>

#include <Moxie.h>
#include <HandlePool.h>

#include <boost/function.h>

moxie::Moxie *moxie::Moxie::moxie_ = nullptr;

bool moxie::Moxie::MoxieInit(boost::function<void ()> UserInitFunc) {
    Instance()->RegisterHandler();
    if (UserInitFunc) {
        UserInitFunc();
    }
}

static moxie::Moxie* Moxie::Instance() {
    if (!moxie_) {
        moxie_ = new (nothrow) Moxie();
    }
    assert(moxie_);
    return moxie_;
}

void moxie::Moxie::RegisterHandler() {
    Eventsops::RegisterEventHandler(EVENT_TYPE_EVENTFD, new EventfdHandle());
    Eventsops::RegisterEventHandler(EVENT_TYPE_TCPCON, new TcpHandle());
    Eventsops::RegisterEventHandler(EVENT_TYPE_TCPSER, new TcpSerHandle());
    Eventsops::RegisterEventHandler(EVENT_TYPE_TIMER, new TimerHandle());
}
