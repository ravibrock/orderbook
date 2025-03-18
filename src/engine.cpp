#include "engine.hpp"

Engine::Engine() : orderbooks() {}

Engine::Engine(std::vector<std::string> markets) : orderbooks() {
    for (const auto& market : markets) {
        this->add_orderbook(market);
    }
}

void Engine::add_orderbook(std::string asset) {
    if (!this->orderbook_exists(asset)) {
        this->orderbooks[asset] = Orderbook();
    }
}

void Engine::remove_orderbook(std::string asset) {
    this->orderbooks.erase(asset);
}

// Returns if an orderbook has been initialized already
bool Engine::orderbook_exists(std::string asset) {
    return this->orderbooks.find(asset) != this->orderbooks.end();
}

// Caller is responsible for checking if the orderbook exists
std::vector<Order> Engine::place_order(Order order) {
    return this->orderbooks[order.asset].place_order(order);
}

// Caller is responsible for checking if the orderbook exists
std::unordered_map<int, int> Engine::get_orders(std::string direction, std::string asset, int price) {
    return this->orderbooks[asset].get_orders(direction, price);
}

// Caller is responsible for checking if the orderbook exists
uint64_t Engine::get_buy_depth(std::string asset) {
    return this->orderbooks[asset].get_buy_depth();
}

// Caller is responsible for checking if the orderbook exists
uint64_t Engine::get_sell_depth(std::string asset) {
    return this->orderbooks[asset].get_sell_depth();
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
