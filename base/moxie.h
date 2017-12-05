#ifndef MOXIE_MOXIE_H
#define MOXIE_MOXIE_H

#define EVENT_TYPE_START        0
#define EVENT_TYPE_TCPCON       1
#define EVENT_TYPE_TCPSER       2
#define EVENT_TYPE_TIMER        3
#define EVENT_TYPE_EVENTFD      4

namespace moxie {

void MoxieDefaultInit() {
}

class Moxie {
public:
    static bool MoxieInit(boost::function<void ()> InitFunc = MoxieDefaultInit);
private:
    static Moxie* Instance();
    void RegisterHandler();

    Moxie *moxie_;
};
}

#endif //MOXIE_MOXIE_H
