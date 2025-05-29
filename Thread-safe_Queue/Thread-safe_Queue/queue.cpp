#include <iostream>
#include "queue.h"

using namespace std;

Queue* init(void) {		//Queue �ʱ�ȭ
	Queue* queue = new Queue();
	if (queue == NULL) return NULL;		//Queue ���� ����
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
	return queue;
}


void release(Queue* queue) {	//Queue ����
	{
		unique_lock<mutex> lock(queue->mtx);
		while (queue->head) {	//Queue�� �����ִ� ��� Node ����
			Node* tmp = queue->head;
			queue->head = (queue->head)->next;
			nfree(tmp);
		}
	}
	queue->cv.notify_all();	//Queue ���� �� ��� ������ �����尡 ���� �ִٸ� �����ֱ�
	delete queue;
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
	unique_lock<mutex> lock(queue->mtx);
	Reply reply = { false, NULL };
	Node* new_node = nalloc(item);
	if (new_node == NULL) return reply;	//Node ���� ����
	if (queue->head == NULL) {
		queue->head = new_node;
		queue->tail = new_node;
	}
	else {	//����� Ž��
		Node* left = queue->head;
		Node* right = queue->tail;
		
		while (left != right && left->next != right) {
			if (item.key > left->item.key) break;
			if (item.key <= right->item.key) break;

			left = left->next;		//head���� ������ �������� Ž��
			right = right->prev;	//tail���� ���� �������� Ž��
		}

		// ���� ��ġ ����
		Node* insert_pos = NULL;

		if (item.key > left->item.key) {	//left �տ� ����
			insert_pos = left->prev;	//left�� ù��° ���� prev�� NULL
		}
		else if (item.key <= right->item.key) {	//right �ڿ� ����
			insert_pos = right;
		}
		else {
			insert_pos = left;	//left �ڿ� ����
		}

		// ���� ��ġ ó��
		if (insert_pos == NULL) {	//�� �տ� ����
			new_node->next = queue->head;
			queue->head->prev = new_node;
			queue->head = new_node;
		}
		else if (insert_pos == queue->tail) {	//�� �ڿ� ����
			new_node->prev = queue->tail;
			queue->tail->next = new_node;
			queue->tail = new_node;
		}
		else {	//left �ڿ� ����
			new_node->next = insert_pos->next;
			new_node->prev = insert_pos;
			insert_pos->next->prev = new_node;
			insert_pos->next = new_node;
		}
	}
	queue->size++;	//���� �� Queue ũ�� ����
	queue->cv.notify_one();

	reply.success = true;
	reply.item = item;
	return reply;
}

Reply dequeue(Queue* queue) {
	unique_lock<mutex> lock(queue->mtx);
	Reply reply = { false, NULL };
	if (queue->size <= 0 || queue->head == NULL) return reply;	//Queue�� �������
	queue->cv.wait(lock, [&] { return queue->head != NULL; });
	if (queue == NULL || queue->head == NULL) return reply;		//��� �� ť�� �����ƴٸ� ����
	Node* rm_node = queue->head;
	Item rm_item = rm_node->item;
	
	queue->head = rm_node->next;	//���� head ����� ���� ���� head ����
	if (queue->head == NULL) queue->tail = NULL;	//Queue�� ��� �ȴٸ� tail�� NULL�� ����
	else queue->head->prev = NULL;		//rm_node�� ����� prev�� NULL�� ����

	reply.success = true;
	reply.item = rm_item;

	nfree(rm_node);		//��� ����
	queue->size--;		//���� �� Queue ũ�� ����
	return reply;
}

Queue* range(Queue* queue, Key start, Key end) {
	unique_lock<mutex> lock(queue->mtx);
	if (start > end) {	//Ű���� ������ 10~20 �� �ƴ� 20~10�̸� ���� ���� �ٲ�
		Key tk = end;
		end = start;
		start = tk;
	}
	if (queue->size <= 0 ||
		start < queue->tail->item.key ||
		end > queue->head->item.key) return NULL;	//Ű���� ������ head~tail�� �ƴϸ� NULL
	Queue* new_queue = init();
	if (new_queue == NULL) return NULL;
	Node* left = queue->head;
	Node* right = queue->tail;
	bool direction = true;
	// left�� right�� �����ϸ� Ž��
	while (left->next != right && left != right) {
		if (left->item.key <= end) break;
		left = left->next;

		if (right->item.key >= start) {
			direction = false; 
			break;
		}
		right = right->prev;
	}
	// ������ Ȯ��
	Node* curr = direction ? left : right;
	Node* tn = NULL;
	// ����
	while (curr != NULL && start <= curr->item.key && curr->item.key <= end) {
		Node* new_node = nclone(curr);

		if (new_queue->head == NULL) {
			new_queue->head = new_node;
			new_queue->tail = new_node;
			tn = new_node;
		}
		else if (direction) {
			tn->next = new_node;
			new_node->prev = tn;
			tn = new_node;
			new_queue->tail = tn;
			new_queue->tail->next = NULL; 
		}
		else {
			tn->prev = new_node;
			new_node->next = tn;
			tn = new_node;
			new_queue->head = tn;
			new_queue->head->prev = NULL; 
		}

		new_queue->size++;
		curr = direction ? curr->next : curr->prev;
	}
	return new_queue;
}
