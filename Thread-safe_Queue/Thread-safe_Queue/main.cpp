#include <iostream>
#include <thread>
#include <atomic>
#include "queue.h"
#include <array>

using namespace std;

// �ʰ��� ���� �׽�Ʈ
// ����: �Ʒ� ����(Operation, Request)�� ������ ��
// ť�� Item�� void*�̹Ƿ� �󸶵��� �޶��� �� ����

#define REQUEST_PER_CLINET	100

atomic<int> sum_key = 0;
atomic<int> sum_value = 0;
//atomic<double> response_time_tot = 0.0;

typedef enum {
	GET,
	SET,
	GETRANGE
} Operation;

typedef struct {
	Operation op;
	Item item;
} Request;

void client_func(Queue* queue, Request requests[], int n_request, int id) {
	unique_lock<mutex> lock(queue->mtx);
	Reply reply = { false, 0 };

	// start_time = .....

	for (int i = 0; i < n_request; i++) {
		if (requests[i].op == GET) {
			reply = dequeue(queue);
			cout << "client " << id << " dequeue reply.item.key: " << reply.item.key << endl;	//��ȯ�� item.key�� Ȯ�� 
		}
		else { // SET
			reply = enqueue(queue, requests[i].item);
			cout << "client " << id << " enqueue reply.item.key: " << reply.item.key << endl;
		}

		if (reply.success) {
			// �ܼ��� ���Ϲ��� Ű ���� ����(�ƹ� �ǹ� ����)
			sum_key += reply.item.key;
			sum_value += (int)reply.item.value; // void*���� �ٽ� int�� ��ȯ

			// ���Ϲ��� key, value �� ����
			// ...����...
		}
		else {
			// noop
		}
	}
	if (id == 1) {
		Queue* ran_q = range(queue, 90, 99);
		Node* ran_n = ran_q->head;
		if (ran_n == NULL) cout << "null" << endl;
		else {
			for (int i = 0; i < 10; i++) {
				cout << "ran_n [" << i << "] : " << ran_n->item.key << endl;
				ran_n = ran_n->next;
			}
		}

		Node* a = queue->head;			//ť ���� �׽�Ʈ
		Node* b = ran_q->head;

		while (a && b) {
			cout << "queue node: " << a << ", ran_q node: " << b << endl;
			a = a->next;
			b = b->next;
		}
		release(ran_q);

	}
	
	// ��¥�� �ʿ��� �� �����ð��� �����ϴ� �ڵ�
	//
	// elapsed_time = finish_time - start_time;
	// finish_time = ....
	// average_response_time = elapsed_time / REQUEST_PER_CLIENT;
	// printf(...average_response_time of client1 = .....);
	// response_time_tot += finish_time - start_time;
}

int main(void) {
	srand((unsigned int)time(NULL));

	// ��ũ�ε� ����(GETRANGE�� �н�)
	Request requests[REQUEST_PER_CLINET];
	for (int i = 0; i < REQUEST_PER_CLINET; i++) {
		requests[i].op = SET;
		requests[i].item.key = i;
		requests[i].item.value = (void*)(rand() % 1000000);
	}
	/*for (int i = REQUEST_PER_CLINET / 2; i < REQUEST_PER_CLINET; i++) {
		requests[i].op = GET;
	}*/

	Queue* queue = init();
	if (queue == NULL) return 0;

	// �ϴ� �� �� ���ε�, �׷��� multi client��� �����ϱ�
	
	array<thread, 32> clients;
	for (int i = 0; i < 32; i++)
		clients[i] = thread(client_func, queue, requests, REQUEST_PER_CLINET, i);

	for (int i = 0; i < 32; i++) {
		clients[i].join();
		cout << "cliends[" << i << "] ������ ����" << endl;
	}

	// �ǹ� ���� �۾�
	cout << "sum of returned keys = " << sum_key << endl;
	cout << "sum of returned values = " << sum_value << endl;

	// ��¥�� �ʿ��� �ڵ�
	// total_average_response_time = total_response_time / n_cleint;
	// printf("total average response time = ....

	//nalloc(), nfree(), nclone() �׽�Ʈ
	//nalloc()
	Item t_item = requests[1].item;
	Node* node = nalloc(t_item);	//Node ����
	if (node == NULL) return 0;
	cout << "node ����" << endl;
	cout << "node->item = (" << node->item.key << ", " << node->item.value << ")" << endl;

	//nfree(), nclone()
	Node* cl_node = nclone(node);
	if (cl_node == NULL) { nfree(node); return 0; }
	cout << "node ����" << endl;
	cout << "cl_node->item = (" << cl_node->item.key << ", " << cl_node->item.value << ")" << endl;
	nfree(node); //���� Node ���� �� ���� Node �������� Ȯ��
	cout << "node ����" << endl;
	cout << "node->item = (" << node->item.key << ", " << node->item.value << ")" << endl;
	cout << "cl_node->item = (" << cl_node->item.key << ", " << cl_node->item.value << ")" << endl;
	nfree(cl_node); //���� Node ����
	cout << "cl_node ����" << endl;
	cout << "node->item = (" << node->item.key << ", " << node->item.value << ")" << endl;
	cout << "cl_node->item = (" << cl_node->item.key << ", " << cl_node->item.value << ")" << endl;

	release(queue);
	return 0;
}