#ifndef SERVER_H
#define SERVER_H

#include <crow.h>
#include <unordered_map>
#include "engine.hpp"

class Server {
    public:
        Server(int port);

    private:
        int port;
        crow::SimpleApp app;
        Engine engine;
        std::unordered_map<std::string, std::string> users;
        crow::response limit_order(Order order);
        crow::response update_user(std::string user_id, std::string callback);
        crow::response get_orders(std::string direction, std::string asset, int price);
        int inform_user(Order fill);
};

#endif // SERVER_H
