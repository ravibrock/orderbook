#include "engine.hpp"

Engine::Engine() {}

Engine::Engine(std::vector<Market> markets) : orderbooks() {
    for (const auto& market : markets) {
        this->add_orderbook(market);
    }
}

void Engine::add_orderbook(Market market) {
    if (!this->orderbook_exists(market.name)) {
        this->orderbooks.emplace(market.name, Orderbook(market.min, market.max));
    }
}

void Engine::remove_orderbook(std::string asset) {
    this->orderbooks.erase(asset);
}

// Returns if an orderbook has been initialized already
bool Engine::orderbook_exists(std::string asset) {
    return this->orderbooks.find(asset) != this->orderbooks.end();
}

Orderbook Engine::get_orderbook(std::string name) {
    return this->orderbooks.find(name)->second;
}

// Caller is responsible for checking if the orderbook exists
std::vector<Order> Engine::place_order(Order order) {
    return this->get_orderbook(order.asset).place_order(order);
}

// Caller is responsible for checking if the orderbook exists
std::unordered_map<int, int> Engine::get_orders(bool direction, std::string asset, int price) {
    return this->get_orderbook(asset).get_orders(direction, price);
}

// Caller is responsible for checking if the orderbook exists
uint64_t Engine::get_buy_depth(std::string asset) {
    return this->get_orderbook(asset).get_buy_depth();
}

// Caller is responsible for checking if the orderbook exists
uint64_t Engine::get_sell_depth(std::string asset) {
    return this->get_orderbook(asset).get_sell_depth();
}

// Caller is responsible for checking if the orderbook exists
int Engine::get_min_price(std::string asset) {
    return this->get_orderbook(asset).get_min_price();
}

// Caller is responsible for checking if the orderbook exists
int Engine::get_max_price(std::string asset) {
    return this->get_orderbook(asset).get_max_price();
}

// Order does not have to exist
std::optional<Order> Engine::cancel_order(int order_id) {
    for (auto& orderbook : this->orderbooks) {
        if (std::optional<Order> order = orderbook.second.cancel_order(order_id)) {
            return order;
        }
    }
    return std::nullopt;
}
