#include <iostream>
#include <thread>
#include <atomic>
#include "queue.h"
#include <array>

using namespace std;

// 초간단 구동 테스트
// 주의: 아래 정의(Operation, Request)는 예시일 뿐
// 큐의 Item은 void*이므로 얼마든지 달라질 수 있음

#define REQUEST_PER_CLINET	10000

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
	Reply reply = { false, 0 };
	// start_time = .....

	for (int i = 0; i < n_request; i++) {
		if (requests[i].op == GET) {
			reply = dequeue(queue);
			//cout << "client " << id << " dequeue reply.item.key: " << reply.item.key << endl;	//반환된 item.key값 확인 
			if (reply.item.value != NULL) { 
				free(reply.item.value);
				reply.item.value = NULL;
			}
		}
		else { // SET
			reply = enqueue(queue, requests[i].item);
			//cout << "client " << id << " enqueue reply.item.key: " << reply.item.key << endl;
		}

		if (reply.success) {
			// 단순히 리턴받은 키 값을 더함(아무 의미 없음)
			sum_key += reply.item.key;
			sum_value += (int)reply.item.value; // void*에서 다시 int로 변환

			// 리턴받은 key, value 값 검증
			// ...생략...
		}
		else {
			// noop
		}
	}
	// 진짜로 필요한 건 지연시간을 측정하는 코드
	//
	// elapsed_time = finish_time - start_time;
	// finish_time = ....
	// average_response_time = elapsed_time / REQUEST_PER_CLIENT;
	// printf(...average_response_time of client1 = .....);
	// response_time_tot += finish_time - start_time;
}

int main(void) {
	srand((unsigned int)time(NULL));

	// 워크로드 생성(GETRANGE는 패스)
	//Request requests[REQUEST_PER_CLINET];
	Request* requests = new Request[REQUEST_PER_CLINET];
	for (int i = 0; i < REQUEST_PER_CLINET / 2; i++) {
		//cout << "SET" << endl;
		requests[i].op = SET;
		requests[i].item.key = i;
		//int size = rand() % 1000000;
		requests[i].item.value = (void*)"abcdefg";
		requests[i].item.value_size = sizeof("abcdefg");
	}
	for (int i = REQUEST_PER_CLINET / 2; i < REQUEST_PER_CLINET; i++) {
		//cout << "GET" << endl;
		requests[i].op = GET;
	}

	Queue* queue = init();
	if (queue == NULL) return 0;

	// 일단 한 개 뿐인데, 그래도 multi client라고 가정하기
	
	array<thread, 32> clients;
	for (int i = 0; i < 32; i++)
		clients[i] = thread(client_func, queue, requests, REQUEST_PER_CLINET, i);

	for (int i = 0; i < 32; i++) {
		clients[i].join();
		cout << "cliends[" << i << "] 스레드 종료" << endl;
	}

	// 의미 없는 작업
	cout << "sum of returned keys = " << sum_key << endl;
	cout << "sum of returned values = " << sum_value << endl;

	// 진짜로 필요한 코드
	// total_average_response_time = total_response_time / n_cleint;
	// printf("total average response time = ....

	//nalloc(), nfree(), nclone() 테스트
	//nalloc()
	Item t_item = requests[1].item;
	Node* node = nalloc(t_item);	//Node 생성
	if (node == NULL) return 0;
	cout << "node 생성" << endl;
	cout << "node->item = (" << node->item.key << ", " << node->item.value << ", " << node->item.value_size << ")" << endl;

	//nfree(), nclone()
	Node* cl_node = nclone(node);
	if (cl_node == NULL) { nfree(node); return 0; }
	cout << "node 복제" << endl;
	cout << "cl_node->item = (" << cl_node->item.key << ", " << cl_node->item.value << ")" << endl;
	nfree(node); //원본 Node 해제 후 복제 Node 변조여부 확인
	cout << "node 해제" << endl;
	cout << "node->item = (" << node->item.key << ", " << node->item.value << ")" << endl;
	cout << "cl_node->item = (" << cl_node->item.key << ", " << cl_node->item.value << ")" << endl;
	nfree(cl_node); //복제 Node 해제
	cout << "cl_node 해제" << endl;
	cout << "node->item = (" << node->item.key << ", " << node->item.value << ")" << endl;
	cout << "cl_node->item = (" << cl_node->item.key << ", " << cl_node->item.value << ")" << endl;

	/*Queue* ran_q = range(queue, 0, 0);
	if (ran_q == NULL) cout << "queue null" << endl;
	Reply ran_ply = dequeue(ran_q);
	cout << ran_ply.item.key << endl;
	Node* ran_n = ran_q->head;
	if (ran_n == NULL) cout << "node null" << endl;
	else {
		for (int i = 0; i < 10; i++) {
			cout << "ran_n [" << i << "] : " << ran_n->item.key << endl;
			ran_n = ran_n->next;
		}
	}*/

	Queue* nqueue = init();
	Item nitem;
	Reply nply;
	for (int i = 0; i < 10; i++) {
		nitem.key = i;
		nitem.value = (void*)"ssss";
		nitem.value_size = sizeof("ssss");
		nply = enqueue(nqueue, nitem);
		cout << nply.item.key << ", " << (char*)nply.item.value << endl;
	}

	Node* tn = nqueue->head;
	for (int i = 0; i < 10; i++) {
		cout << i+1 << "번째 노드 = (" << tn->item.key << ", " << (char*)tn->item.value << ")" << endl;
		tn = tn->next;
	}
	cout << "" << endl;
	char* nstr = _strdup("abcd");
	nitem = { 5, (void*)nstr, (int)strlen(nstr)+1};
	nply = enqueue(nqueue, nitem);
	tn = nqueue->head;
	Node* t_rm = NULL;
	for (int i = 0; i < 10; i++) {
		cout << i + 1 << "번째 노드 = (" << tn->item.key << ", " << (char*)tn->item.value << ")" << endl;
		if (tn->item.key == 5) t_rm = tn;	//nqueue의 5번 key 노드를 가리킴
		tn = tn->next;
	}
	cout << "range 전" << endl;
	Queue* rque = range(nqueue, 5, 9);
	cout << "range 후" << endl;
	tn = rque->head;
	for (int i = 0; i < 5; i++) {
		cout << i + 1 << "번째 노드 = (" << tn->item.key << ", " << (char*)tn->item.value << ")" << endl;
		tn = tn->next;
	}
	cout << "rque->size: " << rque->size << endl;
	cout << "" << endl;
	free(nstr);
	if (t_rm->item.value != NULL) {
		free(t_rm->item.value);
		t_rm->item.value = NULL;
		cout << "nqueue의 5번 key 삭제" << endl;
	}
	tn = rque->head;
	for (int i = 0; i < 5; i++) {
		cout << i + 1 << "번째 노드 = (" << tn->item.key << ", " << (char*)tn->item.value << ")" << endl;
		tn = tn->next;
	}
	
	release(nqueue);
	release(rque);
	release(queue);
	return 0;
}