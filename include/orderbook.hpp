#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <string>
#include <vector>
#include <unordered_map>

enum Direction {
    BUY,
    SELL,
};

struct Order {
    std::string user;
    bool direction;
    std::string asset;
    int quantity;
    int price; // Support negative prices 2020 style
    int order_id;
};

class Orderbook {
public:
    Orderbook(int min_price, int max_price);
    std::vector<Order> place_order(Order order);
    std::optional<Order> cancel_order(int order_id);
    std::unordered_map<int, int> get_orders(std::string direction, int price);
    uint64_t get_buy_depth();
    uint64_t get_sell_depth();
    int get_min_price();
    int get_max_price();

private:
    uint64_t buy_depth;
    uint64_t sell_depth;
    int min;
    int max;
    std::unordered_map<int, std::shared_ptr<Order>> orders;
};

#endif // ORDERBOOK_H
