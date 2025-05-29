# OS_Thread-safe_Queue
2025-1 OS 과제2 Thread-safe Queue

학번 : 20210539
이름 : 김준섭
- queue 기능 init(), release(), nalloc(), nfree(), nclone(), enqueue(), dequeue(), range()까지 전부 구현 완료
- queue의 각 기능들이 멀티 스레드에서 상호 배제가 이루어지도록 enqueue()와 dequeue(), range()에 lock 처리
- dequeue()에서 큐가 비었을 경우 대기 후 enqueue() 후 대기중인 dequeue() 깨워주도록 설계
- queue 해제 시 만약 dequeue()에서 대기 중인 스레드가 남아있다면 스레드를 깨우고 작업을 종료함
- range()로 start <= key <= end 인 key 값을 가진 노드를 가진 복제 큐를 리턴함
