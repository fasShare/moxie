#ifndef MOXIE_RWLOCK_H
#define MOXIE_RWLOCK_H
#include <pthread.h>

namespace moxie {

class Rwlock {
private:
    pthread_rwlock_t lock_;
public:
    Rwlock();
    bool rlock();
    bool tryrlock();
    bool trywlock();
    bool wlock(); 
    bool unlock();
    ~Rwlock();
};

}
#endif // MOXIE_RWLOCK_H
