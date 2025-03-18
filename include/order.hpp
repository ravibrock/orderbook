#ifndef ORDER_H
#define ORDER_H

#include <string>

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

#endif // ORDER_H
