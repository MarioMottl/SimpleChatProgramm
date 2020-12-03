#include "chatwithme.hpp"

/*
Function_name:
    lo2cl(...)
Description:
    local to client
    Handles sending and receiving between the connections
*/
void lo2cl(cppsock::socket *lo, cppsock::socket *cl)
{
    char buf[256];
    ssize_t len;
    while ( (len = lo->recv(buf, sizeof(buf), 0)) > 0 )
    {
        cl->send(buf, len, 0);
        std::cout << lo->getsockname() << " -> " << cl->getsockname() << ":" << buf << std::endl;
    }
    cl->shutdown(SHUT_RDWR);
    cl->close();
    std::cout << "Disconnected" << std::endl;
}

/*
Function_name:
    lo2cl(...)
Description:
    client to local
    Handles sending and receiving between the connections
*/

void cl2lo(cppsock::socket *lo, cppsock::socket *cl)
{
    char buf[256];
    ssize_t len;
    while ( (len = cl->recv(buf, sizeof(buf), 0)) > 0 )
    {
        lo->send(buf, len, 0);
        std::cout << cl->getsockname() << " -> " << lo->getsockname() << ":" << buf << std::endl;
    }
    lo->shutdown(SHUT_RDWR);
    lo->close();
    std::cout << "Disconnected" << std::endl;
}

int main()
{
    int failcounter = 0;
    cppsock::socket listener, lo, cl;
    //waits until the defined port is free
    while(tcp_server_setup(listener, nullptr, connection_port, 0) != 0)
    {
        //throw std::runtime_error("error setting up TCP server");
        std::cout << "Failed to set up server (#" << ++failcounter << ")\r";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    } 
    std::cout << "server listening to " << listener.getsockname() << std::endl;
    listener.accept(lo);
    std::cout << "Client from " << lo.getpeername() << " connected (1/2)" << std::endl;
    listener.accept(cl);
    std::cout << "Client from " << cl.getpeername() << " connected (2/2)" << std::endl;
    listener.close();
    std::thread t1(lo2cl, &lo, &cl), t2(cl2lo, &lo, &cl);
    std::cout << "Clients connected" << std::endl;
    t1.join();
    t2.join();
}