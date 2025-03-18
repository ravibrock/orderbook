#include <cpr/cpr.h>
#include "server.hpp"

// Contructs a new orderbook server
Server::Server(int port, Engine engine) : app(), engine(engine), port(port), users() {
    CROW_ROUTE(app, "/limit/<string>/<string>/<string>/<int>/<int>").methods(crow::HTTPMethod::POST)(
        [this](std::string user, std::string direction, std::string asset, int quantity, int price){
            bool dir;
            if (direction == "buy") {
                dir = BUY;
            } else if (direction == "sell") {
                dir = SELL;
            } else {
                return crow::response(404);
            }
            return this->limit_order(Order{
                user,
                dir,
                asset,
                quantity,
                price,
            });
        }
    );
    CROW_ROUTE(app, "/market/<string>/<string>/<string>/<int>").methods(crow::HTTPMethod::POST)(
        [this](std::string user, std::string direction, std::string asset, int quantity){
            bool dir;
            if (direction == "buy") {
                dir = BUY;
            } else if (direction == "sell") {
                dir = SELL;
            } else {
                return crow::response(404);
            }
            return this->market_order(Order{
                user,
                dir,
                asset,
                quantity,
            });
        }
    );
    CROW_ROUTE(app, "/user/<string>/<string>").methods(crow::HTTPMethod::POST)(
        [this](std::string user_id, std::string callback){
            return this->update_user(user_id, callback);
        }
    );
    CROW_ROUTE(app, "/orders/<string>/<string>/<int>").methods(crow::HTTPMethod::GET)(
        [this](std::string direction, std::string asset, int price){
            return this->get_orders(direction, asset, price);
        }
    );
    CROW_ROUTE(app, "/books/<string>").methods(crow::HTTPMethod::POST)(
        [this](std::string asset){
            return this->add_orderbook(asset);
        }
    );
    CROW_ROUTE(app, "/shutdown").methods(crow::HTTPMethod::POST)(
        [this](){
            return this->shutdown();
        }
    );
}

void Server::start_server() {
    this->app.port(this->port).multithreaded().run();
}

// Places a limit order
crow::response Server::limit_order(Order order) {
    crow::json::wvalue data;
    if (!this->user_exists(order.user)) {
        data["message"] = "user must be registered prior to placing an order";
        return crow::response(401, data);
    }
    if (!this->engine.orderbook_exists(order.asset)) {
        data["message"] = "orderbook does not exist";
        return crow::response(404, data);
    }

    for (Order fill : this->engine.place_order(order)) {
        this->inform_user(fill);
    }
    return crow::response(200);
// Places a market order
crow::response Server::market_order(Order order) {
    if (!this->engine.orderbook_exists(order.asset)) {
        return this->limit_order(order);
    }

    // Ensures that market orders don't "overflow" but lets us still use `limit_order()` functionality
    if (order.direction == BUY) {
        order.quantity = std::min((uint64_t) order.quantity, this->engine.get_sell_depth(order.asset));
    } else {
        order.quantity = std::min((uint64_t) order.quantity, this->engine.get_buy_depth(order.asset));
    }

    order.price = order.direction == BUY ? INT_MAX : INT_MIN;
    return this->limit_order(order);
}

crow::response Server::cancel_order(int order_id) {
    crow::json::wvalue data;
    std::optional<Order> order = this->engine.cancel_order(order_id);
    if (!order) {
        data["message"] = "order not found";
        return crow::response(204, data);
    }
    data["order_id"] = order->order_id;
    data["direction"] = order->direction ? "sell" : "buy";
    data["price"] = order->price;
    data["quantity"] = order->quantity;
    data["asset"] = order->asset;
    data["user_id"] = order->user;
    return crow::response(200, data);
}

// Updates the callback for when a user is filled
crow::response Server::update_user(std::string user_id, std::string callback) {
    bool ret = false;
    crow::json::wvalue data;
    if (this->user_exists(user_id)) ret = true;
    this->users[user_id] = callback;
    data["already_registered"] = ret;
    return crow::response(200, data);
}

// Adds orderbook to the engine
crow::response Server::add_orderbook(std::string asset) {
    this->engine.add_orderbook(asset);
    return crow::response(200);
}

// Pings user when request is fulfilled
int Server::inform_user(Order fill) {
    std::string callback_url = this->users[fill.user];
    crow::json::wvalue data;

    data["user"] = fill.user;
    data["direction"] = fill.direction;
    data["asset"] = fill.asset;
    data["quantity"] = fill.quantity;
    data["price"] = fill.price;
    data["status"] = "filled";
    cpr::Response r = cpr::Post(
        cpr::Url{callback_url},
        cpr::Body{data.dump()},
        cpr::Header{{"Content-Type", "application/json"}}
    );

    return r.status_code;
}

// Gets orders up/down to a certain price
crow::response Server::get_orders(std::string direction, std::string asset, int price) {
    crow::json::wvalue data;
    if (!this->engine.orderbook_exists(asset)) {
        data["message"] = "orderbook does not exist";
        return crow::response(404, data);
    }

    std::unordered_map<int, int> orders = this->engine.get_orders(direction, asset, price);
    for (auto&[price, quantity] : orders) {
        data[std::to_string(price)] = quantity;
    }
    return crow::response(200, data);
}

// Checks if a user exists
bool Server::user_exists(std::string user_id) {
    return this->users.find(user_id) != this->users.end();
}

// Shuts down the server
crow::response Server::shutdown() {
    this->app.stop();
    return crow::response(200);
}
