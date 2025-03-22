#!/usr/bin/env python3
"""
this script tests an orderbook server by placing dozens of buy/sell orders.
it registers several users, adds an orderbook for asset "BTC", and submits a mix of
limit and market orders. finally, it queries the orderbook to verify that the active
order quantities match the expected values.

the test scenario is as follows:
  - register 5 users.
  - add an orderbook for BTC with price bounds 100 and 200.
  - submit 10 limit buy orders (all at price 140) totaling 60 in quantity.
  - submit 10 limit sell orders (all at price 160) totaling 72 in quantity.
    (since buy orders at 140 and sell orders at 160 don’t cross, no matching occurs yet)
  - place a market buy order (by user1) for quantity 40.
    this should match sell orders in fifo order, leaving a total of 32 quantity on sell side.
  - place a market sell order (by user2) for quantity 30.
    this should match buy orders in fifo order, leaving a total of 30 quantity on buy side.
  - finally, the script queries the orderbook via the GET endpoints and asserts that:
      active buy orders at price 140 == 30
      active sell orders at price 160 == 32
"""

import requests
import subprocess
import threading
import time
from http.server import BaseHTTPRequestHandler, HTTPServer
import json

# global list to capture callback notifications
callback_notifications = []

class CallbackHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(content_length)
        try:
            data = json.loads(body)
        except Exception:
            data = body.decode()
        print(f"callback received on {self.path}: {data}")
        callback_notifications.append((self.path, data))
        self.send_response(200)
        self.end_headers()

    def log_message(self, format, *args):
        # suppress default logging
        return

def run_callback_server(port=18081):
    server_address = ("", port)
    httpd = HTTPServer(server_address, CallbackHandler)
    print(f"starting callback server on port {port}")
    httpd.serve_forever()

def start_orderbook_server(port=18080):
    print("starting orderbook server...")
    proc = subprocess.Popen(["../build/orderbook", "--port", str(port)],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # wait for the server to start up
    time.sleep(2)
    return proc

def stop_orderbook_server(proc):
    proc.terminate()
    proc.wait()
    print("orderbook server terminated.")

# functions to interact with server endpoints
def register_user(base_url, user, callback_url):
    url = f"{base_url}/user/{user}/{callback_url}"
    r = requests.post(url)
    print(f"register user {user}: status={r.status_code}, response={r.json()}")
    return r

def add_orderbook(base_url, asset, min_price, max_price):
    url = f"{base_url}/books/{asset}/{min_price}/{max_price}"
    r = requests.post(url)
    print(f"add orderbook for {asset}: status={r.status_code}")
    return r

def place_limit_order(base_url, user, direction, asset, quantity, price):
    url = f"{base_url}/limit/{user}/{direction}/{asset}/{quantity}/{price}"
    r = requests.post(url)
    try:
        resp = r.json()
    except Exception:
        resp = r.text
    print(f"limit order {direction} by {user} for {asset} qty={quantity} price={price}: status={r.status_code}, response={resp}")
    return r, resp

def place_market_order(base_url, user, direction, asset, quantity):
    url = f"{base_url}/market/{user}/{direction}/{asset}/{quantity}"
    r = requests.post(url)
    try:
        resp = r.json()
    except Exception:
        resp = r.text
    print(f"market order {direction} by {user} for {asset} qty={quantity}: status={r.status_code}, response={resp}")
    return r, resp

def get_orders(base_url, direction, asset, price_limit):
    url = f"{base_url}/orders/{direction}/{asset}/{price_limit}"
    r = requests.get(url)
    try:
        resp = r.json()
    except Exception:
        resp = r.text
    print(f"get orders {direction} for {asset} with limit {price_limit}: status={r.status_code}, response={resp}")
    return r, resp

def main():
    base_url = "http://localhost:18080"

    # start callback server in a separate thread
    callback_thread = threading.Thread(target=run_callback_server, args=(18081,), daemon=True)
    callback_thread.start()

    # start orderbook server process
    proc = start_orderbook_server(18080)

    try:
        # register 5 users
        users = ["user1", "user2", "user3", "user4", "user5"]
        for user in users:
            register_user(base_url, user, f"http://localhost:18081/{user}")

        # add orderbook for asset BTC with price bounds 100 and 200
        add_orderbook(base_url, "BTC", 100, 200)

        # -------------------------
        # Place limit buy orders (all at price 140)
        # create 10 buy orders with varying quantities
        buy_orders = [
            {"user": "user1", "quantity": 4},
            {"user": "user2", "quantity": 8},
            {"user": "user3", "quantity": 6},
            {"user": "user4", "quantity": 7},
            {"user": "user5", "quantity": 5},
            {"user": "user1", "quantity": 4},
            {"user": "user2", "quantity": 8},
            {"user": "user3", "quantity": 6},
            {"user": "user4", "quantity": 7},
            {"user": "user5", "quantity": 5},
        ]
        total_buy_quantity = sum(order["quantity"] for order in buy_orders)
        for order in buy_orders:
            place_limit_order(base_url, order["user"], "buy", "BTC", order["quantity"], 140)

        # Place limit sell orders (all at price 160)
        sell_orders = [
            {"user": "user1", "quantity": 5},
            {"user": "user2", "quantity": 10},
            {"user": "user3", "quantity": 8},
            {"user": "user4", "quantity": 7},
            {"user": "user5", "quantity": 6},
            {"user": "user1", "quantity": 5},
            {"user": "user2", "quantity": 10},
            {"user": "user3", "quantity": 8},
            {"user": "user4", "quantity": 7},
            {"user": "user5", "quantity": 6},
        ]
        total_sell_quantity = sum(order["quantity"] for order in sell_orders)
        for order in sell_orders:
            place_limit_order(base_url, order["user"], "sell", "BTC", order["quantity"], 160)

        print(f"initial active buy orders (expected total at price 140): {total_buy_quantity}")
        print(f"initial active sell orders (expected total at price 160): {total_sell_quantity}")

        # note: since buy orders at 140 and sell orders at 160 do not cross, no matching happens yet.

        # -------------------------
        # execute a market buy order to consume some sell orders
        # market buy order will set its price to 200 and match orders from the lowest ask (160)
        market_buy_qty = 40
        place_market_order(base_url, "user1", "buy", "BTC", market_buy_qty)

        # expected matching on sell side (fifo order):
        # sell orders (in order of placement): 5, 10, 8, 7, 6, 5, 10, 8, 7, 6 (total 72)
        # matching steps:
        #  - order1: 5  -> remaining qty: 40 - 5 = 35
        #  - order2: 10 -> remaining qty: 35 - 10 = 25
        #  - order3: 8  -> remaining qty: 25 - 8 = 17
        #  - order4: 7  -> remaining qty: 17 - 7 = 10
        #  - order5: 6  -> remaining qty: 10 - 6 = 4
        #  - order6: 5  -> partially filled: 4 filled, remaining 1
        # orders 7-10 remain untouched.
        expected_remaining_sell = 1 + 10 + 8 + 7 + 6  # = 32
        print(f"expected remaining sell quantity after market buy: {expected_remaining_sell}")

        # -------------------------
        # execute a market sell order to consume some buy orders
        # market sell order will set its price to 100 and match orders from the highest bid (140)
        market_sell_qty = 30
        place_market_order(base_url, "user2", "sell", "BTC", market_sell_qty)
        # expected matching on buy side:
        # buy orders (fifo order): 4,8,6,7,5,4,8,6,7,5 (total 60)
        # matching steps:
        #  - order1: 4  -> remaining: 30 - 4 = 26
        #  - order2: 8  -> remaining: 26 - 8 = 18
        #  - order3: 6  -> remaining: 18 - 6 = 12
        #  - order4: 7  -> remaining: 12 - 7 = 5
        #  - order5: 5  -> exactly filled, remaining: 5 - 5 = 0
        # remaining buy orders (orders 6-10) total: 4+8+6+7+5 = 30
        expected_remaining_buy = total_buy_quantity - 30  # 60 - 30 = 30
        print(f"expected remaining buy quantity after market sell: {expected_remaining_buy}")

        # give some time for the market orders to process fully
        time.sleep(1)

        # -------------------------
        # check active orders using the GET endpoint
        # for buy orders, the best bid is at price 140.
        # the endpoint for buy orders returns orders from hi_bid down to max(price, hi_bid)
        # so call with a lower limit (e.g. 100) to get the best bid level.
        _, buy_resp = get_orders(base_url, "buy", "BTC", 100)
        actual_buy_qty = 0
        if isinstance(buy_resp, dict) and "140" in buy_resp:
            actual_buy_qty = int(buy_resp["140"])
        print(f"actual active buy quantity at price 140: {actual_buy_qty}")
        assert actual_buy_qty == expected_remaining_buy, f"expected buy qty {expected_remaining_buy}, got {actual_buy_qty}"

        # for sell orders, the best ask is at price 160.
        # the endpoint for sell orders returns orders from lo_ask up to min(price, lo_ask).
        _, sell_resp = get_orders(base_url, "sell", "BTC", 200)
        actual_sell_qty = 0
        if isinstance(sell_resp, dict) and "160" in sell_resp:
            actual_sell_qty = int(sell_resp["160"])
        print(f"actual active sell quantity at price 160: {actual_sell_qty}")
        assert actual_sell_qty == expected_remaining_sell, f"expected sell qty {expected_remaining_sell}, got {actual_sell_qty}"

        print("orderbook active orders match expected values.")

    finally:
        # shutdown the server
        shutdown_url = f"{base_url}/shutdown"
        try:
            r = requests.post(shutdown_url)
            print(f"shutdown request: status={r.status_code}")
        except Exception as e:
            print("error during shutdown:", e)
        stop_orderbook_server(proc)
        print("test complete.")

if __name__ == "__main__":
    main()
