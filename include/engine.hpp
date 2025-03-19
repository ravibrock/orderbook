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
    Engine(const std::vector<Market>& markets);
    void add_orderbook(const Market& market);
    void remove_orderbook(const std::string& asset);
    bool orderbook_exists(const std::string& asset);
    uint64_t get_buy_depth(const std::string& asset);
    uint64_t get_sell_depth(const std::string& asset);
    int get_min_price(const std::string& asset);
    int get_max_price(const std::string& asset);
    std::optional<Order> cancel_order(int order_id);
    std::vector<Order> place_order(Order& order);
    std::unordered_map<int, int> get_orders(bool direction, const std::string& asset, int price);

private:
    std::unordered_map<int, std::string> id_to_asset;
    std::unordered_map<std::string, Orderbook> orderbooks;
    Orderbook& get_orderbook(const std::string& name);
};

#endif // ENGINE_H
