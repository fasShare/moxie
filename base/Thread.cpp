#include <exception>


#include <Thread.h>
#include <Log.h>

moxie::Thread::Thread(){
}

moxie::Thread::Thread(const std::string& name) :
    name_(name){

    }

moxie::Thread::Thread(boost::function<void ()> threadFunc) :
    Thread(threadFunc, "") {
    }

moxie::Thread::Thread(boost::function<void ()> threadFunc, const std::string name) :
    threadId_(0),
    name_(name),
    threadFunc_(threadFunc) {
}

bool moxie::Thread::setThreadFunc(boost::function<void ()> threadFunc) {
    if (threadFunc_)
        return false;
    threadFunc_ = threadFunc;
    return true;
}

bool moxie::Thread::join() {
    errno = ::pthread_join(threadId_, NULL);
    if (errno != 0) {
        LOGGER_SYSERR("ERROR pthread_join : " <<  ::strerror(errno));
        return false;
    }
    return true;
}

bool moxie::Thread::MainThread() {
    return gettid() == ::getpid();
}

bool moxie::Thread::start() {
    errno = ::pthread_create(&threadId_, NULL, &run, this);
    if (errno != 0) {
        LOGGER_SYSERR("ERROR pthread_create : " << ::strerror(errno));
        return false;
    }
    return true;
}

void* moxie::run(void *obj) {
    Thread *thread = static_cast<Thread*>(obj);
    thread->ThreadFunc();
    return NULL;
}

void moxie::Thread::ThreadFunc() {
    if (!this->threadFunc_) {
        LOGGER_ERROR("Thread no Function object!");
        return;
    }
    try {
        threadFunc_();
    } catch (const std::exception &ex) {
        LOGGER_ERROR("exception : " << ex.what());
    }
}

std::string moxie::Thread::getName() {
    return name_;
}

void moxie::Thread::setName(const std::string& name) {
    this->name_ = name;
}

moxie::Thread::~Thread() {

}


