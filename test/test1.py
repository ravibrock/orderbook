import requests

# change this to the appropriate base url for your api
BASE_URL = "http://0.0.0.0:8080"

def test_limit_order():
    # endpoint: /limit/<user>/<direction>/<asset>/<quantity>/<price>
    url = f"{BASE_URL}/limit/test_user/buy/btc/10/50000"
    response = requests.post(url)
    print("limit order response:", response.status_code, response.text)

def test_market_order():
    # endpoint: /market/<user>/<direction>/<asset>/<quantity>
    url = f"{BASE_URL}/market/test_user/sell/btc/5"
    response = requests.post(url)
    print("market order response:", response.status_code, response.text)

def test_update_user():
    # endpoint: /user/<user_id>/<callback>
    url = f"{BASE_URL}/user/test_user/http://callback.url"
    response = requests.post(url)
    print("update user response:", response.status_code, response.text)

def test_get_orders():
    # endpoint: /orders/<direction>/<asset>/<price>
    url = f"{BASE_URL}/orders/sell/btc/60000"
    response = requests.get(url)
    print("get orders response:", response.status_code, response.text)
    url = f"{BASE_URL}/orders/buy/btc/0"
    response = requests.get(url)
    print("get orders response:", response.status_code, response.text)

def test_add_orderbook():
    # endpoint: /books/<asset>/<min_price>/<max_price>
    url = f"{BASE_URL}/books/btc/30000/60000"
    response = requests.post(url)
    print("add orderbook response:", response.status_code, response.text)

def test_cancel_order():
    # endpoint: /cancel/<order_id>
    url = f"{BASE_URL}/cancel/1"
    response = requests.post(url)
    print("cancel order response:", response.status_code, response.text)

def test_shutdown():
    # endpoint: /shutdown
    url = f"{BASE_URL}/shutdown"
    response = requests.post(url)
    print("shutdown response:", response.status_code, response.text)

def main():
    print("testing add orderbook endpoint:")
    test_add_orderbook()

    print("\ntesting limit order endpoint (unregistered):")
    test_limit_order()

    print("\ntesting market order endpoint (unregistered):")
    test_market_order()

    print("\ntesting update user endpoint:")
    test_update_user()

    print("\ntesting get orders endpoint (empty):")
    test_get_orders()

    print("\ntesting limit order endpoint (registered):")
    test_limit_order()

    print("\ntesting limit order endpoint (registered):")
    test_limit_order()

    print("\ntesting market order endpoint (registered):")
    test_market_order()

    print("\ntesting get orders endpoint (orders exist):")
    test_get_orders()

    print("\ntesting cancel order endpoint:")
    test_cancel_order()

    print("\ntesting get orders endpoint (orders exist):")
    test_get_orders()

    print("\ntesting shutdown endpoint:")
    test_shutdown()

if __name__ == "__main__":
    main()
