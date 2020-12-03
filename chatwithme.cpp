#include "chatwithme.hpp"

void getoptions(int argc, char **argv,user &u)
{
    //<chatwithme> <-k key> <-m my_msg_type> <-r rcvr_msg_type> <-u user_name>
    //./chatwithme -k 12 -m 23 -r 34 -u KieselsteinKurt
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
    bool success = (cppsock::tcp_client_connect(this->sock, nullptr, connection_port) == 0);
    recv_thread = std::thread(print_received, this);
    return success;
}

void user::debug()
{
    std::cout<< "key: " << this->key <<"\n"<< "my_msg_type: " << this ->my_msg_type<<"\n"<< "rcvr_msg_type: " << this->rcvr_msg_type << "\n" << "user_name: " << this->user_name << std::endl;
}

void user::send(const std::string &message)
{
    this->sock.send(message.data(), message.size(), 0);
}

void print_received(user *u)
{
    char buf[256];
    while(u->sock.recv(&buf, sizeof(buf), 0) > 0)
    {
        std::cout <<"\rOther>> "<< buf << "                                          " << "\n" << u->user_name << ">> ";
        std::cout.flush();
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
    u.connect();
    std::cout << u.user_name << ">> ";
    while(running)
    {
        for(size_t i=0; i<input.size(); i++) input.at(i) = 0;
        std::getline(std::cin, input);
        if((input == "bye") || (input == "quit"))
        {
            u.disconnect();
            running = false;
        }
        else
        {
            std::cout << u.user_name << ">> ";
            u.send(input);
        }
    }
    u.join();
}