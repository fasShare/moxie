#ifndef FAS_HANDLEPOOL_H
#define FAS_HANDLEPOOL_H
#include <map>

#include <Handler.h>
#include <Events.h>

namespace fas {

class HandlePool {
public:
    static bool AddHandler(Events::type type, Handler *handler) {
        return Instance()->addHandler(type, handler);
    }

    static Handler *GetHandler(Events::type type) {
        return Instance()->getHandler(type);
    }
private:
    bool addHandler(Events::type type, Handler *handler);
    Handler *getHandler(Events::type type);
    static HandlePool* Instance();
    
    static HandlePool *pool_;
    std::map<Events::type, Handler*> handlers_;
};

}

#endif //FAS_HANDLEPOOL_H
