#include <algorithm>
#include "orderbook.hpp"

Orderbook::Orderbook(int min, int max) :
    buy_depth(0),
    sell_depth(0),
    min_price(min),
    max_price(max),
    lo_ask(max+1),
    hi_bid(min-1),
    book(max - min + 1)
{}

int Orderbook::get_min_price() {
    return this->min_price;
}

int Orderbook::get_max_price() {
    return this->max_price;
}

std::optional<Order> Orderbook::cancel_order(int order_id) {
    if (this->prices.find(order_id) == this->prices.end()) {
        return std::nullopt;
    }
    std::optional<Order> ret = this->access_book(this->prices[order_id]).remove(order_id);

    if (ret->direction == BUY) {
        this->buy_depth -= ret->quantity;
        if (this->buy_depth == 0) {
            this->hi_bid = this->min_price - 1;
        } else {
            while (this->access_book(this->hi_bid).isEmpty()) this->hi_bid--;
        }
    } else {
        this->sell_depth -= ret->quantity;
        if (this->sell_depth == 0) {
            this->lo_ask = this->max_price + 1;
        } else {
            while (this->access_book(this->lo_ask).isEmpty()) this->lo_ask++;
        }
    }

    this->prices.erase(order_id);
    return ret;
}

uint64_t Orderbook::get_buy_depth() {
    return this->buy_depth;
}

uint64_t Orderbook::get_sell_depth() {
    return this->sell_depth;
}

Order Orderbook::copy_order(const Order& order) {
    return order;
}

// Places order and returns orders that were matched
std::vector<Order> Orderbook::place_order(Order order) {
    std::vector<Order> orders;
    if (order.quantity == 0) { // Edge case for market orders hitting empty book
        return orders;
    } else if (order.direction == BUY) {
        while (order.price >= this->lo_ask) {
            Order cur = this->access_book(this->lo_ask).dequeue(); // Matched order
            if (order.quantity == cur.quantity) {
                this->prices.erase(cur.order_id); // Delete cur from prices dict
                order.price = cur.price; // Update price to cur

                // Add to return dict of matched orders
                orders.push_back(cur);
                orders.push_back(order);

                this->sell_depth -= cur.quantity; // Delete cur's depth from sell_depth

                // Update lo_ask
                if (this->sell_depth == 0) {
                    this->lo_ask = this->max_price + 1;
                } else {
                    while (this->access_book(this->lo_ask).isEmpty()) this->lo_ask++;
                }

                return orders; // Break out since we're done
            } else if (order.quantity < cur.quantity) {
                // We fill at order's qty and cur's price
                Order nxt = this->copy_order(cur);
                nxt.quantity = order.quantity;

                // Add to return dict of matched orders
                orders.push_back(nxt);
                orders.push_back(order);

                // Update sell_depth and cur's quantity
                cur.quantity -= order.quantity;
                this->sell_depth -= order.quantity;
                this->access_book(order.price).push(cur); // Toss back cur

                return orders; // Break out since we're done
            } else { // order.quantity > cur.quantity
                Order part = this->copy_order(order);

                this->prices.erase(cur.order_id); // Delete cur from prices dict

                // We fill at cur's qty and price
                part.quantity = cur.quantity;
                part.price = cur.price;

                // Add to return dict of matched orders
                orders.push_back(cur);
                orders.push_back(part);

                // We're now looking for fewer orders and sell_depth is lower
                order.quantity -= cur.quantity;
                this->sell_depth -= cur.quantity;

                // Update lo_ask
                if (this->sell_depth == 0) {
                    this->lo_ask = this->max_price + 1;
                } else {
                    while (this->access_book(this->lo_ask).isEmpty()) this->lo_ask++;
                }
            }
        }
        // If we get here, we need to add the order to the book
        this->access_book(order.price).push(order);
        this->prices[order.order_id] = order.price;
        this->buy_depth += order.quantity;
        this->hi_bid = std::max(order.price, this->hi_bid);
        return orders;
    } else {
        while (order.price <= this->hi_bid) {
            Order cur = this->access_book(this->hi_bid).dequeue(); // Matched order
            if (order.quantity == cur.quantity) {
                this->prices.erase(cur.order_id); // Delete cur from prices dict
                order.price = cur.price; // Update price to cur

                // Add to return dict of matched orders
                orders.emplace_back(cur);
                orders.emplace_back(order);

                this->buy_depth -= cur.quantity; // Delete cur's depth from buy_depth

                // Update hi_bid
                if (this->buy_depth == 0) {
                    this->hi_bid = this->min_price - 1;
                } else {
                    while (this->access_book(this->hi_bid).isEmpty()) this->hi_bid--;
                }

                return orders; // Break out since we're done
            } else if (order.quantity < cur.quantity) {
                // We fill at order's qty and cur's price
                Order nxt = this->copy_order(cur);
                nxt.quantity = order.quantity;

                // Add to return dict of matched orders
                orders.push_back(nxt);
                orders.push_back(order);

                // Update buy_depth and cur's quantity
                cur.quantity -= order.quantity;
                this->buy_depth -= order.quantity;
                this->access_book(order.price).push(cur); // Toss back cur

                return orders; // Break out since we're done
            } else { // order.quantity > cur.quantity
                Order part = this->copy_order(order);

                this->prices.erase(cur.order_id); // Delete cur from prices dict

                // We fill at cur's qty and price
                part.quantity = cur.quantity;
                part.price = cur.price;

                // Add to return dict of matched orders
                orders.push_back(cur);
                orders.push_back(part);

                // We're now looking for fewer orders and buy_depth is lower
                order.quantity -= cur.quantity;
                this->buy_depth -= cur.quantity;

                // Update hi_bid
                if (this->buy_depth == 0) {
                    this->hi_bid = this->min_price - 1;
                } else {
                    while (this->access_book(this->hi_bid).isEmpty()) this->hi_bid--;
                }
            }
        }
        // If we get here, we need to add the order to the book
        this->access_book(order.price).push(order);
        this->prices[order.order_id] = order.price;
        this->sell_depth += order.quantity;
        this->lo_ask = std::min(order.price, this->lo_ask);
        return orders;
    }
}

std::unordered_map<int, int> Orderbook::get_orders(bool direction, int price) {
    std::unordered_map<int, int> ret;

    if (direction == BUY) {
        price = std::max(price, this->hi_bid);
        for (int i = this->hi_bid; i >= price; i--) {
            if (!this->access_book(i).isEmpty()) {
                ret[i] = this->access_book(i).get_quantity();
            }
        }
    } else {
        price = std::min(price, this->lo_ask);
        for (int i = this->lo_ask; i <= price; i++) {
            if (!this->access_book(i).isEmpty()) {
                ret[i] = this->access_book(i).get_quantity();
            }
        }
    }

    return ret;
}

Queue& Orderbook::access_book(int price) {
    return this->book[price - this->min_price];
}
