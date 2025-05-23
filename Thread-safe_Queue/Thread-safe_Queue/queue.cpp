#include <iostream>
#include "queue.h"

using namespace std;

Queue* init(void) {		//Queue 초기화
	Queue* queue = new Queue();
	if (queue == NULL) return NULL;		//Queue 생성 실패
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
	return queue;
}


void release(Queue* queue) {	//Queue 해제
	{
		unique_lock<mutex> lock(queue->mtx);
		while (queue->head) {	//Queue에 남아있는 모든 Node 해제
			Node* tmp = queue->head;
			queue->head = (queue->head)->next;
			nfree(tmp);
		}
	}
	delete queue;
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
mutex mtx;

Reply enqueue(Queue* queue, Item item) {
	unique_lock<mutex> lock(queue->mtx);
	Reply reply = { false, NULL };
	Node* new_node = nalloc(item);
	if (new_node == NULL) return reply;	//Node 생성 실패
	if (queue->head == NULL) {
		queue->head = new_node;
		queue->tail = new_node;
	}
	else {	//양방향 탐색
		Node* left = queue->head;
		Node* right = queue->tail;
		
		while (left != right && left->next != right) {
			if (item.key > left->item.key) break;
			if (item.key <= right->item.key) break;

			left = left->next;		//head에서 오른쪽 방향으로 탐색
			right = right->prev;	//tail에서 왼쪽 방향으로 탐색
		}

		// 삽입 위치 결정
		Node* insert_pos = NULL;

		if (item.key > left->item.key) {	//left 앞에 삽입
			insert_pos = left->prev;	//left가 첫번째 노드면 prev는 NULL
		}
		else if (item.key <= right->item.key) {	//right 뒤에 삽입
			insert_pos = right;
		}
		else {
			insert_pos = left;	//left 뒤에 삽입
		}

		// 삽입 위치 처리
		if (insert_pos == NULL) {	//맨 앞에 삽입
			new_node->next = queue->head;
			queue->head->prev = new_node;
			queue->head = new_node;
		}
		else if (insert_pos == queue->tail) {	//맨 뒤에 삽입
			new_node->prev = queue->tail;
			queue->tail->next = new_node;
			queue->tail = new_node;
		}
		else {	//left 뒤에 삽입
			new_node->next = insert_pos->next;
			new_node->prev = insert_pos;
			insert_pos->next->prev = new_node;
			insert_pos->next = new_node;
		}
	}
	queue->size++;	//삽입 후 Queue 크기 증가
	queue->cv.notify_one();

	reply.success = true;
	reply.item = item;
	return reply;
}

Reply dequeue(Queue* queue) {
	unique_lock<mutex> lock(queue->mtx);
	Reply reply = { false, NULL };
	if (queue->size <= 0 && queue->head == NULL) return reply;	//Queue가 비어있음

	queue->cv.wait(lock, [&] { return queue->head != NULL; });
	Node* rm_node = queue->head;
	Item rm_item = rm_node->item;
	
	queue->head = rm_node->next;	//기존 head 노드의 다음 노드로 head 변경
	if (queue->head == NULL) queue->tail = NULL;	//Queue가 비게 된다면 tail도 NULL로 변경
	else queue->head->prev = NULL;		//rm_node와 연결된 prev를 NULL로 변경

	reply.success = true;
	reply.item = rm_item;

	nfree(rm_node);		//노드 해제
	queue->size--;		//삭제 후 Queue 크기 감소
	return reply;
}

Queue* range(Queue* queue, Key start, Key end) {
	return NULL;
}
