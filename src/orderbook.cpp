#include "orderbook.hpp"

Orderbook::Orderbook(int min, int max) : buy_depth(0), sell_depth(0), orders(), min(min), max(max) {}

int Orderbook::get_min_price() {
    return this->min;
}

int Orderbook::get_max_price() {
    return this->max;
}

std::optional<Order> Orderbook::cancel_order(int order_id) {
    return Order{};
}

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
