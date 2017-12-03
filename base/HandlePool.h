#ifndef FAS_HANDLEPOOL_H
#define FAS_HANDLEPOOL_H
#include <map>

#include <Handler.h>
#include <Events.h>

namespace fas {

class HandlePool {
public:
    static HandlePool* Instance();

    Handler *getHandler(Events::type type);

    bool addHandler(Events::type type, Handler *handler);
private:
    static HandlePool *pool_;
    std::map<Events::type, Handler*> handlers_;
};

}

#endif //FAS_HANDLEPOOL_H
