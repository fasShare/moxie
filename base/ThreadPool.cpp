#include <ThreadPool.h>
#include <memory>

moxie::ThreadPool::ThreadPool(int threadNum, boost::function<void ()> func, const std::string name) :
    curThreadNum_(0),
    threadNum_(threadNum),
    threads_(),
    name_(name),
    threadFunc_(func),
    tid_(gettid()),
    started_(false) {
    assert(threadNum >= 0);
}

void moxie::ThreadPool::updateThreadFunc(boost::function<void ()> func) {
    assertInOwnerThread();
    assert(!started_);
    this->threadFunc_ = func;
}

void moxie::ThreadPool::updateThreadNum(int newNum) {
    assertInOwnerThread();
    assert(!started_);
    assert(newNum >= 0);
    threadNum_ = newNum;
}

void moxie::ThreadPool::assertInOwnerThread() {
    assert(gettid() == tid_);
}

bool moxie::ThreadPool::start() {
    assertInOwnerThread();
    assert(!started_);
    for(int i = 0; i < threadNum_; i++) {
        threads_.push_back(std::make_shared<moxie::Thread>(threadFunc_));
    }
    int count = 0;
    for(auto iter = threads_.begin(); iter != threads_.end(); iter++) {
        if ((*iter)->start()) {
            count++;
        } else {
            break;
        }
    }
    started_ = true;
    return count != threadNum_ ? false : true;
}


moxie::ThreadPool::~ThreadPool() {
    if (started_) {
        for(auto iter = threads_.begin(); iter < threads_.end(); iter++) {
            (*iter)->join();
        }
    }
}
