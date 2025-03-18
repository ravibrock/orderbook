#ifndef SERVER_H
#define SERVER_H

#include <crow.h>
#include <unordered_map>
#include "engine.hpp"

class Server {
public:
    Server(int port, Engine engine);
    void start_server();

private:
    int port;
    crow::SimpleApp app;
    Engine engine;
    bool user_exists(std::string user_id);
    std::unordered_map<std::string, std::string> users;
    crow::response limit_order(Order order);
    crow::response market_order(Order order);
    crow::response cancel_order(int order_id);
    crow::response update_user(std::string user_id, std::string callback);
    crow::response get_orders(bool direction, std::string asset, int price);
    crow::response add_orderbook(Market market);
    int cur_order_idx = 0;
    int inform_user(Order fill);
    crow::response shutdown();
};

#endif // SERVER_H
