#include "queue.hpp"

Queue::Queue() : head(nullptr), tail(nullptr), quantity(0) {}

Queue::~Queue() {
    while (!this->isEmpty()) {
        this->dequeue();
    }
}

void Queue::enqueue(Order order) {
    ListNode* node = new ListNode{order};
    this->orders[order.order_id] = node;
    if (this->isEmpty()) {
        this->head = node;
        this->tail = node;
    } else {
        this->tail->next = node;
        node->prev = this->tail;
        this->tail = node;
    }
    this->quantity += order.quantity;
}

void Queue::push(Order order) {
    ListNode* node = new ListNode{order};
    this->orders[order.order_id] = node;
    if (this->isEmpty()) {
        this->head = node;
        this->tail = node;
    } else {
        this->head->prev = node;
        node->next = this->head;
        this->head = node;
    }
    this->quantity += order.quantity;
}

Order Queue::remove(int order_id) {
    if (this->orders.find(order_id) == this->orders.end()) {
        throw std::out_of_range("Order not found");
    }
    ListNode* node = this->orders[order_id];
    Order ret = node->order;

    if (node == this->head && node == this->tail) {  // only one node
        this->head = this->tail = nullptr;
    } else if (node == this->head) {  // removing head
        this->head = this->head->next;
        this->head->prev = nullptr;
    } else if (node == this->tail) {  // removing tail
        this->tail = this->tail->prev;
        this->tail->next = nullptr;
    } else {  // removing a middle node
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    this->quantity -= ret.quantity;
    this->orders.erase(order_id);
    delete node;
    return ret;
}

Order Queue::dequeue() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty");
    }
    return this->remove(this->head->order.order_id);
}

Order Queue::get_front() {
    if (this->isEmpty()) {
        throw std::out_of_range("Queue is empty");
    }
    return this->head->order;
}

uint64_t Queue::get_quantity() {
    return this->quantity;
}

bool Queue::isEmpty() {
    return this->head == nullptr;
}
