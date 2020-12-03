#ifndef CHATWITHME_HPP_INCLUDED
#define CHATWITHME_HPP_INCLUDED

/*
Include Files
*/
#include "cppsock/cppsock.hpp"
#include <iostream>
#include <deque>
#include <map>
#include <string>
#include <thread>
#include <unistd.h>
#include <utility>

/*
Definitions
*/
constexpr uint16_t connection_port = 8080;

class user
{
    public:
        std::string key;
        std::string my_msg_type;
        std::string rcvr_msg_type;
        std::string user_name;
        cppsock::socket sock;
        std::thread recv_thread;
    public:
        user();
        user(const user&) = default;
        bool connect();
        void send(const std::string &message);
        void debug();
        void disconnect(void);
        void join();
};
void print_received(cppsock::socket*);

#endif