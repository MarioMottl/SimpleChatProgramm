/*
Author: Clemens Pruggmayer, Mario Mottl
Class: 5BHEL
*/
#ifndef CHATWITHME_HPP_INCLUDED
#define CHATWITHME_HPP_INCLUDED

/*
Include Files
*/
#include "cppsock/cppsock.hpp"
#include <unistd.h>
#include <iostream>
#include <utility>
#include <atomic>
#include <thread>
#include <string>
#include <deque>
#include <map>
#include <set>

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
void print_received(user*);

/*
A simple self written getopt()
*/
void getoptions(int argc, char **argv,user &u);

#endif