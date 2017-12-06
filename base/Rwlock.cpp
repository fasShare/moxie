#include <errno.h>

#include "Rwlock.h"

moxie::Rwlock::Rwlock() {
    lock_ = PTHREAD_RWLOCK_INITIALIZER;
}

moxie::Rwlock::~Rwlock(){
    ::pthread_rwlock_destroy(&lock_);
}

bool moxie::Rwlock::rlock() {
    int ret = ::pthread_rwlock_rdlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool moxie::Rwlock::tryrlock() {
    int ret = ::pthread_rwlock_tryrdlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool moxie::Rwlock::trywlock() {
    int ret = ::pthread_rwlock_wrlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool moxie::Rwlock::wlock() {
    int ret = ::pthread_rwlock_wrlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}

bool moxie::Rwlock::unlock() {
    int ret = ::pthread_rwlock_unlock(&lock_);
    errno = ret == 0 ? errno : ret;
    return ret == 0;
}



