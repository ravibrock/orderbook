#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <vector>
#include <unordered_map>
#include "order.hpp"
#include "queue.hpp"

class Orderbook {
public:
    Orderbook(int min_price, int max_price);
    std::vector<Order> place_order(Order& order);
    std::optional<Order> cancel_order(int order_id);
    std::unordered_map<int, int> get_orders(bool direction, int price);
    uint64_t get_buy_depth();
    uint64_t get_sell_depth();
    int get_min_price();
    int get_max_price();

private:
    uint64_t buy_depth; // Buy depth
    uint64_t sell_depth; // Sell depth
    int min_price; // Min price
    int max_price; // Max price
    int lo_ask; // Lowest ask
    int hi_bid; // Highest bid
    std::vector<Queue> book; // Array of queues for orders
    std::unordered_map<int, int> prices; // Map of order IDs to prices
    Queue& access_book(int price);
};

#endif // ORDERBOOK_H
