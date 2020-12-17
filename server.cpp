#include "chatwithme.hpp"

std::atomic_bool running;

struct connection_details
{
    std::string key;
    std::string message_type;
    std::string receiver_type;
};
bool operator<(const connection_details &lhs, const connection_details &rhs)
{
    if(lhs.key < rhs.key) return true;
    else if(lhs.key > rhs.key) return false;
    if(lhs.message_type < rhs.message_type) return true;
    else if(lhs.message_type > rhs.message_type) return false;
    if(lhs.receiver_type < rhs.receiver_type) return true;
    else if(lhs.receiver_type > rhs.receiver_type) return false;
    return false;
}
using waiting_map = std::map<connection_details, cppsock::socket>;

struct proxy_details
{
    std::thread t1, t2;
    cppsock::socket s1, s2;
    bool running;
};
bool operator<(const proxy_details &lhs, const proxy_details &rhs)
{
    if(lhs.s1.getpeername() < rhs.s1.getpeername()) return true;
    else if(!(lhs.s1.getpeername() < rhs.s1.getpeername())) return false;
    if(lhs.s2.getpeername() < rhs.s2.getpeername()) return true;
    else if(!(lhs.s2.getpeername() < rhs.s2.getpeername())) return false;
    return false;
}
using proxy_set = std::set<proxy_details>;

std::ostream& operator<< (std::ostream &o, const connection_details &d)
{
    return o << "key: " << d.key << ", msg_type: " << d.message_type << ", recv_type: " << d.receiver_type;
}

bool matches(const connection_details &waiting, const connection_details &matching);
void match(waiting_map &waiting, proxy_set &proxy, connection_details &add, cppsock::socket &sock);
//bool is_waiting(waiting_map &map, const connection_details &test);

bool matches(const connection_details &waiting, const connection_details &matching)
{
    return ((waiting.key == matching.key) && (waiting.message_type == matching.receiver_type) && (waiting.receiver_type == matching.message_type));
}

void read_params(cppsock::socket &con, connection_details &details)
{
    bool done = false;
    char recv_buf;
    while (!done)
    {
        con.recv(&recv_buf, sizeof(recv_buf), 0);
        details.key.push_back(recv_buf);
        if(recv_buf == '\0') done = true;
    }
    done = false;
    while (!done)
    {
        con.recv(&recv_buf, sizeof(recv_buf), 0);
        details.message_type.push_back(recv_buf);
        if(recv_buf == '\0') done = true;
    }
    done = false;
    while (!done)
    {
        con.recv(&recv_buf, sizeof(recv_buf), 0);
        details.receiver_type.push_back(recv_buf);
        if(recv_buf == '\0') done = true;
    }
}

bool is_waiting(waiting_map &map, const connection_details &test)
{
    // iterate over all waiting clients
    for(waiting_map::iterator iter = map.begin(); iter != map.end(); iter++)
    {
        // if client matches, then one is already waiting
        if(matches(iter->first, test))
        {
            return true;
        }
    }
    return false;
}

void proxy(cppsock::socket *dst, cppsock::socket *src, proxy_details *proxy_buf)
{
    char buf[256];
    ssize_t bytes_read;
    std::clog << "sending data from " << src->getpeername() << " to " << dst->getpeername() << std::endl;
    do
    {
        bytes_read = src->recv(buf, sizeof(buf), 0);
        if(bytes_read > 0)
        {
            dst->send(buf, bytes_read, 0);
        }
    } while (bytes_read > 0);
    src->close();
    proxy_buf->running = false;
}

void match(waiting_map &waiting, proxy_set &proxy_s, connection_details &add, cppsock::socket sock)
{
    cppsock::socket waiting_sock;
    proxy_details proxy_buf;
    if(!is_waiting(waiting, add)) // add client to waiting
    {
        //std::clog << "adding waiting client " << add << std::endl;
        waiting.insert(std::pair<connection_details, cppsock::socket>(add, std::move(sock) ));
        add.key.clear();
        add.message_type.clear();
        add.receiver_type.clear();
    }
    else
    {
        //std::clog << "matched waiting client " << add << std::endl;
        std::swap(add.message_type, add.receiver_type); // swap because swapping is required before retrieving the value
        waiting_sock = std::move(waiting.at(add));
        waiting.erase(add);
        proxy_buf.running = true;
        std::clog << "matched " << waiting_sock.getpeername() << " with " << sock.getpeername() << std::endl;
        // move socket into socket details, move proxy details into set, start threads 
        proxy_buf.s1 = std::move(waiting_sock);
        proxy_buf.s2 = std::move(sock);
        std::pair<proxy_set::iterator, bool> ret = proxy_s.insert(std::move(proxy_buf));
        proxy_details *iter = const_cast<proxy_details*>(&*ret.first);
        iter->t1 = std::thread(proxy, &iter->s1, &iter->s2, iter);
        iter->t2 = std::thread(proxy, &iter->s2, &iter->s1, iter);
    }
}

void accept_handler(waiting_map *waiting, proxy_set *proxy, cppsock::socket *listener)
{
    cppsock::socket con;
    connection_details details;
    while(running)
    {
        listener->accept(con);
        //std::clog << "connected from " << con.getpeername() << std::endl;
        read_params(con, details);
        //std::clog << "params: " << details << std::endl;
        match(*waiting, *proxy, details, std::move(con));
    }
}

int main()
{
    cppsock::socket listener;
    waiting_map waiting;
    proxy_set proxy;
    running = true;
    while(cppsock::tcp_server_setup(listener, nullptr, connection_port, 1) != 0)
    {
        std::cout << "server binding failed\r";
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    std::cout << "server set up to listen at " << listener.getsockname() << std::endl;
    std::thread accepter(accept_handler, &waiting, &proxy, &listener);
    accepter.join();
    return 0;
}