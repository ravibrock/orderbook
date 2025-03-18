#include "engine.hpp"

Engine::Engine() : orderbooks() {}

void Engine::add_orderbook(std::string asset) {
    if (!this->orderbooks.contains(asset)) {
        this->orderbooks[asset] = Orderbook();
    }
}

void Engine::remove_orderbook(std::string asset) {
    this->orderbooks.erase(asset);
}

// Returns if an orderbook has been initialized already
bool Engine::orderbook_exists(std::string asset) {
    return this->orderbooks.contains(asset);
}

// Caller is responsible for checking if the orderbook exists
std::vector<Order> Engine::place_order(Order order) {
    return this->orderbooks[order.asset].place_order(order);
}

// Caller is responsible for checking if the orderbook exists
std::unordered_map<int, int> Engine::get_orders(std::string direction, std::string asset, int price) {
    return this->orderbooks[asset].get_orders(direction, price);
}
