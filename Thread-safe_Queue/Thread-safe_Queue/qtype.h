#ifndef _QTYPE_H  // header guard
#define _QTYPE_H
#include <mutex>
#include <condition_variable>
#include <atomic>

// ==========�� ������ ���� ����==========

typedef unsigned int Key;  // ���� Ŭ���� ���� �켱����
typedef void* Value;

typedef struct {
    Key key;
    Value value;
} Item;

typedef struct {
    bool success;   // true: ����, false: ����
    Item item;
    // �ʵ� �߰� ����
} Reply;

typedef struct node_t {
    Item item;
    struct node_t* next; //���� ���
    struct node_t* prev; //���� ���
    // �ʵ� �߰� ����
} Node;

typedef struct {
    Node* head;
    Node* tail;
    // �ʵ� �߰� ����
    std::atomic<int> size;
    std::mutex mtx;
    std::condition_variable cv;
} Queue;

// ���� �����Ӱ� �߰�/����: ���ο� �ڷ��� ���� ��

#endif
