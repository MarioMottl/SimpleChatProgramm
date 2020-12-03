#include "chatwithme.hpp"

void getoptions(int argc, char **argv,user &u)
{
    //<chatwithme> <-k key> <-m my_msg_type> <-r rcvr_msg_type> <-u user_name>
    char argument[1024];
    for(int i = 1; i < argc-1; i++)
    {
        strcpy(argument,argv[i]);
        if (strcmp(argument, "-k") == 0 )
        {
            u.key = argv[i+1];
        }
        else if(strcmp(argument, "-m") == 0)
        {
            u.my_msg_type = argv[i+1];
        }
        else if(strcmp(argument, "-r") == 0)
        {
            u.rcvr_msg_type = argv[i+1];
        }
        else if(strcmp(argument, "-u") == 0)
        {
            u.user_name = argv[i+1];
        }
    }
}

user::user()
{
    return;
}

bool user::connect()
{
    recv_thread = std::thread(print_received, &this->sock);
    return cppsock::tcp_client_connect(this->sock, nullptr, connection_port) == 0;
}

void user::debug()
{
    std::cout<< "key: " << this->key <<"\n"<< "my_msg_type: " << this ->my_msg_type<<"\n"<< "rcvr_msg_type: " << this->rcvr_msg_type << "\n" << "user_name: " << this->user_name << std::endl;
}

void user::send(const std::string &message)
{
    this->sock.send(message.data(), message.size(), 0);
}

void print_received(cppsock::socket *s)
{
    char buf;
    while(s->recv(&buf, sizeof(buf), 0) > 0)
    {
        std::cout << buf << std::endl;
    }
}

void user::join()
{
    recv_thread.join();
}

void user::disconnect(void)
{
    this->sock.shutdown(SHUT_RDWR);
    this->sock.close();
}

int main(int argc, char **argv)
{
    user u;
    bool running = true;
    std::string input;
    getoptions(argc,argv,u);
    u.debug();
    u.connect();
    while(running)
    {
        std::getline(std::cin, input);
        std::
    }
    u.join();
}