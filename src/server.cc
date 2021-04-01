#include "codec.h"

#include <evpp/event_loop.h>
#include <evpp/tcp_server.h>

#include <mutex>
#include <iostream>
#include <stdio.h>
#include <set>

class ChatServer {
public:
    ChatServer(evpp::EventLoop* loop,
               const std::string& addr)
        : server_(loop, addr, "ChatServer", 1),
        codec_(std::bind(&ChatServer::OnStringMessage, this, std::placeholders::_1, std::placeholders::_2)) {
        server_.SetConnectionCallback(
            std::bind(&ChatServer::OnConnection, this, std::placeholders::_1));
        server_.SetMessageCallback(
            std::bind(&LengthHeaderCodec::OnMessage, &codec_, std::placeholders::_1, std::placeholders::_2));
    }

    void Start() {
        server_.Init();
        server_.Start();
    }

private:
    void OnConnection(const evpp::TCPConnPtr& conn) {
        LOG_INFO << conn->AddrToString() << " is " << (conn->IsConnected() ? "UP" : "DOWN");
        if (conn->IsConnected()) {
            connections_.insert(conn);
        } else {
            connections_.erase(conn);
        }
    }

    void OnStringMessage(const evpp::TCPConnPtr&,
                         const std::string& message) {
        for (ConnectionList::iterator it = connections_.begin();
             it != connections_.end();
             ++it) {
            std::string long_msg;
            for(size_t i = 0; i< 2000;i++){
                long_msg+=message;
            }
            codec_.Send(*it, long_msg);

        }
    }

public:
    typedef std::set<evpp::TCPConnPtr> ConnectionList;
    evpp::TCPServer server_;
    LengthHeaderCodec codec_;
    ConnectionList connections_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s port\n", argv[0]);
        return -1;
    }

    evpp::EventLoop loop;
    std::string addr = std::string("0.0.0.0:") + argv[1];
    ChatServer server(&loop, addr);
    auto f1 = [&]{
        server.Start();
        loop.Run();
    };

    auto f2 = [&]{
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            LOG(WARNING) << server.connections_.size();
            if(server.connections_.size() == 0) {
                continue;
            }
            for (std::set<evpp::TCPConnPtr>::iterator it = server.connections_.begin();
                 it != server.connections_.end();
                 ++it) {
                std::string long_msg = "heart beat";

                server.codec_.Send(*it, long_msg);

            }
        }

    };

    std::thread t1(f1);
    std::thread  t2(f2);
    t1.join();
    t2.join();
    return 0;
}

