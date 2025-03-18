#include "orderbook.hpp"

/*
TODO:
- Add matching functionality
    - Array of custom DLL deques
- Add cancellation functionality
    - Update order hashmap to point to each order's node
    - This lets us remove efficiently
- Implement get_orders
*/
Orderbook::Orderbook(int min, int max) :
    buy_depth(0),
    sell_depth(0),
    min_price(min),
    max_price(max),
    lo_ask(min-1),
    hi_bid(max+1),
    book(max - min)
{}

int Orderbook::get_min_price() {
    return this->min_price;
}

int Orderbook::get_max_price() {
    return this->max_price;
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
