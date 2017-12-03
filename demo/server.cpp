#include <TcpServer.h>
#include <HandlePool.h>
#include <TcpHandler.h>
#include <Events.h>

using namespace fas;

int main() {
    TcpServer *ser = new TcpServer(NetAddress(AF_INET, 6686, "127.0.0.1"), 10);
    HandlePool::Instance()->addHandler(Events::type::TCP, new TcpHandle());
    HandlePool::Instance()->addHandler(Events::type::TCP_SERVER, ser);
    ser->start();
    return 0;
}
