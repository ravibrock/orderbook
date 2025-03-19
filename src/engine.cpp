#include "engine.hpp"

Engine::Engine() {}

Engine::Engine(const std::vector<Market>& markets) : orderbooks() {
    for (const auto& market : markets) {
        this->add_orderbook(market);
    }
}

void Engine::add_orderbook(const Market& market) {
    if (!this->orderbook_exists(market.name)) {
        this->orderbooks.emplace(market.name, Orderbook(market.min, market.max));
    }
}

void Engine::remove_orderbook(const std::string& asset) {
    this->orderbooks.erase(asset);
}

// Returns if an orderbook has been initialized already
bool Engine::orderbook_exists(const std::string& asset) {
    return this->orderbooks.find(asset) != this->orderbooks.end();
}

Orderbook& Engine::get_orderbook(const std::string& name) {
    return this->orderbooks.at(name);
}

// Caller is responsible for checking if the orderbook exists
std::vector<Order> Engine::place_order(Order& order) {
    this->id_to_asset[order.order_id] = order.asset;
    return this->get_orderbook(order.asset).place_order(order);
}

// Caller is responsible for checking if the orderbook exists
std::unordered_map<int, int> Engine::get_orders(bool direction, const std::string& asset, int price) {
    return this->get_orderbook(asset).get_orders(direction, price);
}

// Caller is responsible for checking if the orderbook exists
uint64_t Engine::get_buy_depth(const std::string& asset) {
    return this->get_orderbook(asset).get_buy_depth();
}

// Caller is responsible for checking if the orderbook exists
uint64_t Engine::get_sell_depth(const std::string& asset) {
    return this->get_orderbook(asset).get_sell_depth();
}

// Caller is responsible for checking if the orderbook exists
int Engine::get_min_price(const std::string& asset) {
    return this->get_orderbook(asset).get_min_price();
}

// Caller is responsible for checking if the orderbook exists
int Engine::get_max_price(const std::string& asset) {
    return this->get_orderbook(asset).get_max_price();
}

// Order does not have to exist
std::optional<Order> Engine::cancel_order(int order_id) {
    auto it = this->id_to_asset.find(order_id);
    if (it == this->id_to_asset.end()) {
        return std::nullopt; // order id not found
    }
    return this->get_orderbook(it->second).cancel_order(order_id);
}
