#ifndef ENGINE_H
#define ENGINE_H

#include <unordered_map>
#include <vector>
#include "orderbook.hpp"

struct Order {
    std::string user;
    std::string direction;
    std::string asset;
    int quantity;
    int price;
};

class Engine {
public:
    Engine();
    void add_orderbook(std::string asset);
    bool orderbook_exists(std::string asset);
    std::vector<Order> place_order(Order order);

private:
    std::unordered_map<std::string, Orderbook> orderbooks;
};

#endif // ENGINE_H
