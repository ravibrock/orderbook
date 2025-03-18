#ifndef ENGINE_H
#define ENGINE_H

#include <unordered_map>
#include <vector>
#include "orderbook.hpp"

class Engine {
public:
    Engine();
    Engine(std::vector<std::string> markets);
    void add_orderbook(std::string asset);
    void remove_orderbook(std::string asset);
    bool orderbook_exists(std::string asset);
    uint64_t get_buy_depth(std::string asset);
    uint64_t get_sell_depth(std::string asset);
    std::optional<Order> cancel_order(int order_id);
    std::vector<Order> place_order(Order order);
    std::unordered_map<int, int> get_orders(std::string direction, std::string asset, int price);

private:
    std::unordered_map<std::string, Orderbook> orderbooks;
};

#endif // ENGINE_H
