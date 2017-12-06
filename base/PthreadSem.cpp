#include <PthreadSem.h>

moxie::PthreadSem::PthreadSem(const char *name, int oflag, mode_t mode, unsigned int value) :
    vaild_(false),
    sem_(SEM_FAILED),
    name_(name),
    value_(value) {
    sem_ = sem_open(name_.c_str(), oflag, mode, value_);
    if (SEM_FAILED == sem_) {
       vaild_ = false; 
    }
    vaild_ = true;
}

moxie::PthreadSem::~PthreadSem() {
    vaild_ = false;
    sem_destroy(sem_);
}

bool moxie::PthreadSem::init(unsigned int value) {
    if (sem_init(sem_, 0, value) == -1) {
        vaild_ = false;
        return false;
    }
    value_ = value;
    vaild_ = true;
    return true;
}

bool moxie::PthreadSem::increment() {
    if ((!vaild_) || (sem_post(sem_) < 0)) {
        return false;
    }
    return true;
}

bool moxie::PthreadSem::decrement() {
    if ((!vaild_) && (sem_wait(sem_) < 0)) {
        return false;
    }
    return true;
}

bool moxie::PthreadSem::trydecrement() {
    if ((!vaild_) && (sem_trywait(sem_) < 0)) {
        return false;
    }
    return true;
}

std::string moxie::PthreadSem::getName() const {
    return name_;
}

bool moxie::PthreadSem::isVaild() const {
    return vaild_;
}

