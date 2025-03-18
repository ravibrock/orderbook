#ifndef QUEUE_H
#define QUEUE_H

#include <unordered_map>
#include "order.hpp"

struct ListNode {
    Order order;
    ListNode* next;
    ListNode* prev;
};

class Queue {
public:
    Queue();
    ~Queue();
    void enqueue(Order order);
    void push(Order order);
    Order remove(int order_id);
    Order dequeue();
    Order get_front();
    uint64_t get_quantity();
    bool isEmpty();

private:
    ListNode* head;
    ListNode* tail;
    uint64_t quantity;
    std::unordered_map<int, ListNode*> orders;
};

#endif // QUEUE_H
