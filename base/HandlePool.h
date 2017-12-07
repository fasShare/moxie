#ifndef MOXIE_HANDLEPOOL_H
#define MOXIE_HANDLEPOOL_H
#include <map>

#include <Handler.h>
#include <Events.h>

namespace moxie {

class HandlePool {
public:
    static bool AddHandler(int type, Handler *handler) {
        return Instance()->addHandler(type, handler);
    }

    static Handler *GetHandler(int type) {
        return Instance()->getHandler(type);
    }
private:
    bool addHandler(int type, Handler *handler);
    Handler *getHandler(int type);
    static HandlePool* Instance();
    
    static HandlePool *pool_;
    std::map<int, Handler*> handlers_;
};

}

#endif //MOXIE_HANDLEPOOL_H
