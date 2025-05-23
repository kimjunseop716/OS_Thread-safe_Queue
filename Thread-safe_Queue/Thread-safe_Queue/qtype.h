#ifndef _QTYPE_H  // header guard
#define _QTYPE_H
#include <mutex>
#include <condition_variable>
#include <atomic>

// ==========이 파일은 수정 가능==========

typedef unsigned int Key;  // 값이 클수록 높은 우선순위
typedef void* Value;

typedef struct {
    Key key;
    Value value;
} Item;

typedef struct {
    bool success;   // true: 성공, false: 실패
    Item item;
    // 필드 추가 가능
} Reply;

typedef struct node_t {
    Item item;
    struct node_t* next; //다음 노드
    struct node_t* prev; //이전 노드
    // 필드 추가 가능
} Node;

typedef struct {
    Node* head;
    Node* tail;
    // 필드 추가 가능
    std::atomic<int> size;
    std::mutex mtx;
    std::condition_variable cv;
} Queue;

// 이후 자유롭게 추가/수정: 새로운 자료형 정의 등

#endif
