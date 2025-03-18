#include "orderbook.hpp"

Orderbook::Orderbook() : buy_depth(0), sell_depth(0) {}

uint64_t Orderbook::get_buy_depth() {
    return this->buy_depth;
}

uint64_t Orderbook::get_sell_depth() {
    return this->sell_depth;
}

std::vector<Order> Orderbook::place_order(Order order) {
    return std::vector<Order>();
}

std::unordered_map<int, int> Orderbook::get_orders(std::string direction, int price) {
    return std::unordered_map<int, int>();
}
