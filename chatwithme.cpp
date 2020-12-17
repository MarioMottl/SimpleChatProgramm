/*
Author: Clemens Pruggmayer, Mario Mottl
Class: 5BHEL
*/

#include "chatwithme.hpp"

/*
Function_name:
    getoptions(...)
Description:
        A simple self written getopt() for easier usage
        loops through all argv's and if a key is detected the following string will be used
*/
void getoptions(int argc, char **argv,user &u)
{
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

/*
Function_name: 
    user()
Description:
    Constructor of class user
*/
user::user()
{
    return;
}

/*
Function_name:
    connect()
Description:
    connects the client to the server that is hosted on localhost
    starts a thread for receiving any messages from the other participant
*/
bool user::connect()
{
    bool success = (cppsock::tcp_client_connect(this->sock, nullptr, connection_port) == 0);
    if(!success) return false;
    std::cout << "connected sucessfully" << std::endl;
    //this->send(this->key + "\0" + this->my_msg_type + "\0" + this->rcvr_msg_type + "\0");
    this->send(this->key);
    this->send(this->my_msg_type);
    this->send(this->rcvr_msg_type);
    recv_thread = std::thread(print_received, this);
    return success;
}

/*
Function_name:
    debug()
Description:
    A simple debug message to see if getoptions(...) works as intented
*/
void user::debug()
{
    std::cout<< "key: " << this->key <<"\n"<< "my_msg_type: " << this ->my_msg_type<<"\n"<< "rcvr_msg_type: " << this->rcvr_msg_type << "\n" << "user_name: " << this->user_name << std::endl;
}

/*
Function_name:
    send(...)
Description:
    Takes a message and sends it to the server, the server then forwards it to the 
    other chat participant
*/
void user::send(const std::string &message)
{
    this->sock.send(message.data(), message.size()+1, 0);
}

/*
Function_name:
    print_received(...)
Description:
    Prints the received messages into the console
*/
void print_received(user *u)
{
    char buf[256];
    while(u->sock.recv(&buf, sizeof(buf), 0) > 0)
    {
        std::cout <<"\rOther>> "<< buf << "                                          " << "\n" << u->user_name << ">> ";
        std::cout.flush();
    }
}

/*
Function_name:
    join()
Description:
    Joins the thread
*/
void user::join()
{
    recv_thread.join();
}

/*
Function_name:
    disconnect()
Description:
    Disconnects the user from the server and closes the socket connection
*/
void user::disconnect(void)
{
    this->sock.shutdown(SHUT_RDWR);
    this->sock.close();
}

/*
Function_name:
    main(...)
Description:
    Creates an user with the command line arguments, and handle the sending and
    receiving of messages
*/
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