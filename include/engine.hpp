#ifndef ENGINE_H
#define ENGINE_H

#include <unordered_map>
#include <vector>
#include "orderbook.hpp"

struct Market {
    std::string name;
    int min;
    int max;
};

class Engine {
public:
    Engine();
    Engine(std::vector<Market> markets);
    void add_orderbook(Market market);
    void remove_orderbook(std::string asset);
    bool orderbook_exists(std::string asset);
    uint64_t get_buy_depth(std::string asset);
    uint64_t get_sell_depth(std::string asset);
    int get_min_price(std::string asset);
    int get_max_price(std::string asset);
    std::optional<Order> cancel_order(int order_id);
    std::vector<Order> place_order(Order order);
    std::unordered_map<int, int> get_orders(bool direction, std::string asset, int price);

private:
    std::unordered_map<int, std::string> id_to_asset;
    std::unordered_map<std::string, Orderbook> orderbooks;
    Orderbook get_orderbook(std::string name);
};

#endif // ENGINE_H
