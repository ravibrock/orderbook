#!/usr/bin/env python3
"""
this script tests the orderbook server (compiled from server.cpp and orderbook.cpp).
it assumes you have an executable named "./orderbook_server" that takes a port number (e.g., 18080).
the script uses a dummy callback server (on port 18081) to catch callback notifications.
each test prints the response and uses asserts to verify expected behavior.
"""

import json
import subprocess
import threading
import time
from http.server import BaseHTTPRequestHandler, HTTPServer

import requests

# global list to store callback notifications
callback_notifications = []

class CallbackHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        # read the posted json data
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length)
        try:
            data = json.loads(body)
        except Exception as e:
            data = body.decode()
        # log the received callback with its path
        print(f"callback received on {self.path}: {data}")
        callback_notifications.append((self.path, data))
        self.send_response(200)
        self.end_headers()

    def log_message(self, format, *args):
        # override to suppress default logging
        return

def run_callback_server(port=18081):
    server_address = ('', port)
    httpd = HTTPServer(server_address, CallbackHandler)
    print(f"starting callback server on port {port}")
    httpd.serve_forever()

def start_orderbook_server(port=18080):
    # assume the orderbook server executable is in the current directory named "./orderbook_server"
    print("starting orderbook server...")
    proc = subprocess.Popen(["../build/orderbook", "--port", str(port)],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # give the server time to start up
    time.sleep(2)
    return proc

def stop_orderbook_server(proc):
    proc.terminate()
    proc.wait()
    print("orderbook server terminated.")

# test functions for each endpoint

def test_register_user(base_url, user, callback_url):
    url = f"{base_url}/user/{user}/{callback_url}"
    r = requests.post(url)
    print(f"register user '{user}' with callback '{callback_url}': status={r.status_code}, response={r.json()}")
    assert r.status_code == 200

def test_add_orderbook(base_url, asset, min_price, max_price):
    url = f"{base_url}/books/{asset}/{min_price}/{max_price}"
    r = requests.post(url)
    print(f"add orderbook for asset '{asset}' with min={min_price} and max={max_price}: status={r.status_code}")
    assert r.status_code == 200

def test_limit_order(base_url, user, direction, asset, quantity, price, expected_status=200):
    url = f"{base_url}/limit/{user}/{direction}/{asset}/{quantity}/{price}"
    r = requests.post(url)
    try:
        resp_json = r.json()
    except Exception:
        resp_json = r.text
    print(f"limit order by '{user}' ({direction} {asset} qty={quantity} price={price}): status={r.status_code}, response={resp_json}")
    assert r.status_code == expected_status
    return resp_json if r.status_code == 200 else None

def test_market_order(base_url, user, direction, asset, quantity, expected_status=200):
    url = f"{base_url}/market/{user}/{direction}/{asset}/{quantity}"
    r = requests.post(url)
    try:
        resp_json = r.json()
    except Exception:
        resp_json = r.text
    print(f"market order by '{user}' ({direction} {asset} qty={quantity}): status={r.status_code}, response={resp_json}")
    assert r.status_code == expected_status
    return resp_json if r.status_code == 200 else None

def test_get_orders(base_url, direction, asset, price):
    url = f"{base_url}/orders/{direction}/{asset}/{price}"
    r = requests.get(url)
    try:
        resp_json = r.json()
    except Exception:
        resp_json = r.text
    print(f"get orders for {direction} '{asset}' at price {price}: status={r.status_code}, response={resp_json}")
    return resp_json

def test_cancel_order(base_url, order_id, expected_status=200):
    url = f"{base_url}/cancel/{order_id}"
    r = requests.post(url)
    try:
        resp_json = r.json()
    except Exception:
        resp_json = r.text
    print(f"cancel order id {order_id}: status={r.status_code}, response={resp_json}")
    assert r.status_code == expected_status

def test_shutdown(base_url):
    url = f"{base_url}/shutdown"
    r = requests.post(url)
    print(f"shutdown request: status={r.status_code}")
    assert r.status_code == 200

def main():
    base_url = "http://localhost:18080"

    # start the dummy callback server in a separate thread
    callback_thread = threading.Thread(target=run_callback_server, args=(18081,), daemon=True)
    callback_thread.start()

    # start the orderbook server process
    proc = start_orderbook_server(port=18080)

    try:
        # register two users with their callback endpoints
        test_register_user(base_url, "user1", "http://localhost:18081/user1")
        test_register_user(base_url, "user2", "http://localhost:18081/user2")

        # add an orderbook for asset "BTC" with price bounds 100 and 200
        test_add_orderbook(base_url, "BTC", 100, 200)

        # try a limit order with an unregistered user (should fail with 401)
        test_limit_order(base_url, "unregistered", "buy", "BTC", 10, 150, expected_status=401)

        # place a sell limit order by user1: quantity=10 at price=150
        resp_sell = test_limit_order(base_url, "user1", "sell", "BTC", 10, 150)
        order_id_sell = resp_sell.get("order_id")
        print(f"user1 placed sell limit order with id: {order_id_sell}")

        # place a buy market order by user2: quantity=10, should match user1's sell order
        resp_buy = test_market_order(base_url, "user2", "buy", "BTC", 10)
        order_id_buy = resp_buy.get("order_id")
        print(f"user2 placed buy market order with id: {order_id_buy}")

        # give some time for the server to send callbacks
        time.sleep(1)
        print("\ncollected callback notifications:")
        for path, data in callback_notifications:
            print(f"  callback to {path}: {data}")

        # check remaining sell orders at price 150 (expected to be empty due to matching)
        orders_after_match = test_get_orders(base_url, "sell", "BTC", 150)
        print(f"remaining sell orders at price 150 (expected empty): {orders_after_match}")

        # place another sell limit order (for cancellation test)
        resp_sell2 = test_limit_order(base_url, "user1", "sell", "BTC", 5, 160)
        order_id_sell2 = resp_sell2.get("order_id")
        print(f"user1 placed another sell limit order with id: {order_id_sell2} (to be cancelled)")

        # cancel the newly placed order
        test_cancel_order(base_url, order_id_sell2)

        # check orders at price 160 to ensure cancellation (should be empty)
        orders_after_cancel = test_get_orders(base_url, "sell", "BTC", 160)
        print(f"remaining sell orders at price 160 after cancellation (expected empty): {orders_after_cancel}")

    finally:
        # shutdown the orderbook server
        test_shutdown(base_url)
        stop_orderbook_server(proc)
        print("test complete.")

if __name__ == "__main__":
    main()
