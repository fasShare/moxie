#include <strings.h>

#include <Events.h>

fas::Events::Events(const int fd, uint32_t events) :
    fd_(fd),
    state_(state::NEW),
    type_(type::TYPEINVAILD),
    eorigin_(events),
    mutable_(kNoneEvent) {
}

fas::Events::Events(const Events& events) :
    Events(events.fd_, events.eorigin_) {
}

//下面这两个构造主要用与polling返回时
fas::Events::Events(const struct epoll_event& events) :
    Events(events.data.fd, events.events){
}

fas::Events::Events(const struct pollfd& events) :
    Events(events.fd, events.revents) {
}

bool fas::Events::reset() {
    fd_ = -1;
    type_ = type::TYPEINVAILD;
    state_ = state::INVAILD;
    eorigin_ = kNoneEvent;
    mutable_ = kNoneEvent;
    return true;
}

fas::Events::type fas::Events::getType() const {
    return type_;
}

void fas::Events::setType(type tp) {
    type_ = tp;
}

bool fas::Events::invaild() const {
    return state_ == state::INVAILD;
}

bool fas::Events::ismod() const {
    return state_ == state::MOD;
}

bool fas::Events::isdel() const {
    return state_ == state::DEL;
}

bool fas::Events::isnew() const {
    return state_ == state::NEW;
}

bool fas::Events::isused() const {
    return state_ == state::USED;
}

int fas::Events::getFd() const{
    return fd_;
}

long fas::Events::getTid() const {
    return tid_;
}

void fas::Events::setTid(long tid) {
    tid_ = tid;
}

fas::Events::state fas::Events::getState() const {
    return state_;
}

void fas::Events::setState(fas::Events::state sta) {
    state_ = sta;
}

uint32_t fas::Events::getEvents() const{
    return eorigin_;
}

void fas::Events::updateEvents(uint32_t events) {
    eorigin_ = events;
    state_ = state::MOD;
}

void fas::Events::addEvent(uint32_t event) {
    eorigin_ |= event;
    state_ = state::MOD;
}

void fas::Events::deleteEvent(uint32_t event) {
    eorigin_ &= ~event;
    state_ = state::MOD;
}

void fas::Events::addEventMutable(uint32_t event) {
    mutable_ |= event;
    state_ = state::MOD;
}

void fas::Events::deleteEventMutable(uint32_t event) {
    mutable_ &= ~event;
    state_ = state::MOD;
}

void fas::Events::setMutable(uint32_t event) {
    mutable_ = event;
}

uint32_t fas::Events::getMutable() const {
    return mutable_;
}

bool fas::Events::isRead() {
    return containsAtLeastOneEvents(mutable_, POLLIN | POLLPRI | POLLRDHUP);
}
bool fas::Events::isWrite() {
    return containsAtLeastOneEvents(mutable_, POLLOUT);
}
bool fas::Events::isError() {
    return containsAtLeastOneEvents(mutable_, POLLERR | POLLNVAL);
}
bool fas::Events::isClose() {
    bool ret =  containsAtLeastOneEvents(mutable_, POLLHUP) 
        && !containsAtLeastOneEvents(mutable_, POLLIN);
    return ret;
}

bool fas::Events::originRead() {
    return containsAtLeastOneEvents(eorigin_, POLLIN | POLLPRI | POLLRDHUP);
}
bool fas::Events::originWrite() {
    return containsAtLeastOneEvents(eorigin_, POLLOUT);
}

bool fas::Events::containsEvents(uint32_t emutable, uint32_t events) const {
    return ((emutable & events) == events);
}

bool fas::Events::containsAtLeastOneEvents(uint32_t emutable, uint32_t events) const {
    return ((emutable & events) != 0);
}

struct epoll_event fas::Events::epollEvents() {
    struct epoll_event events;
    bzero(&events, sizeof(struct epoll_event));
    events.data.fd = this->getFd();
    events.events = this->getEvents();
    return events;
}

struct pollfd fas::Events::pollEvents() {
    struct pollfd events;
    bzero(&events, sizeof(struct pollfd));
    events.fd = this->getFd();
    events.events = this->getEvents();
    return events;
}

fas::Events::~Events() {
    LOGGER_TRACE("Events destroyed.");
}

std::ostream& fas::operator<<(std::ostream& os, Events& events) {
    os << "fd = " << events.getFd();
    return os;
}
