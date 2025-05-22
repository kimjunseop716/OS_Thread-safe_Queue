#include <iostream>
#include "queue.h"


Queue* init(void) {		//Queue 생성
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	if (queue == NULL) return NULL;		//Queue 생성 실패
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
	return queue;
}


void release(Queue* queue) {	//Queue 해제
	free(queue);
	return;
}


Node* nalloc(Item item) {
	// Node 생성, item으로 초기화
	Node* node = (Node*)malloc(sizeof(Node));
	if (node == NULL) {
		return NULL;	// Node 생성 실패
	}
	node->item = item;
	node->next = NULL;
	node->prev = NULL;
	return node;
}


void nfree(Node* node) {
	free(node);
	return;
}


Node* nclone(Node* node) {
	Node* new_node = nalloc(node->item); //Node 생성
	if (new_node == NULL) {
		return NULL;	//Node 생성 실패
	}
	new_node->next = node->next;	//node가 가르키는 next 주소
	new_node->prev = node->prev;	//node가 가르키는 prev 주소

	return new_node;
}


Reply enqueue(Queue* queue, Item item) {
	Reply reply = { false, NULL };
	return reply;
}

Reply dequeue(Queue* queue) {
	Reply reply = { false, NULL };
	return reply;
}

Queue* range(Queue* queue, Key start, Key end) {
	return NULL;
}
