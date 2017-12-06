#include <Timer.h>
#include <Log.h>

moxie::Timer::Timer(const moxie::TimerCallback& cb, moxie::Timestamp when, double interval) :
    callback_(cb),
    expiration_(when),
    interval_(interval),
    repeat_(interval > 0.0),
    state_(Timer::STATE::ADDED) {
}

void moxie::Timer::run() const {
    callback_();
}

moxie::Timestamp moxie::Timer::getExpiration() const {
    return expiration_;
}

bool moxie::Timer::getRepeat() const {
    return repeat_;
}

moxie::Timer::STATE moxie::Timer::getState() const {
    return state_;
}

void moxie::Timer::setState(Timer::STATE state) {
    state_ = state;
}

void moxie::Timer::restart(moxie::Timestamp now) {
    if (repeat_) {
        expiration_ = moxie::addTime(now, interval_);
    }
    else {
        expiration_ = moxie::Timestamp::invalid();
    }
}

moxie::Timer::~Timer() {
    LOGGER_TRACE("Destroy one Timer!");
}
