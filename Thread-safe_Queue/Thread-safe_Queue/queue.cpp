#include <iostream>
#include "queue.h"


Queue* init(void) {		//Queue ����
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	if (queue == NULL) return NULL;		//Queue ���� ����
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
	return queue;
}


void release(Queue* queue) {	//Queue ����
	free(queue);
	return;
}


Node* nalloc(Item item) {
	// Node ����, item���� �ʱ�ȭ
	Node* node = (Node*)malloc(sizeof(Node));
	if (node == NULL) {
		return NULL;	// Node ���� ����
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
	Node* new_node = nalloc(node->item); //Node ����
	if (new_node == NULL) {
		return NULL;	//Node ���� ����
	}
	new_node->next = node->next;	//node�� ����Ű�� next �ּ�
	new_node->prev = node->prev;	//node�� ����Ű�� prev �ּ�

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
