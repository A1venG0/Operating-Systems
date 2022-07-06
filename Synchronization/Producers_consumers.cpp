// ДОБАВИТЬ ПРОВЕРКУ НА 1 ЭЛЕМЕНТ И РАЗНЫЕ МЬЮТЕКСЫ
#include <bits/stdc++.h>
#include <pthread.h>
#include <ctime>
#include <windows.h>

#define NUM_OF_CONSUMERS 3
#define NUM_OF_PRODUCERS 3
#define BUFF_SIZE 4

using namespace std;

pthread_cond_t buffNotEmpty = PTHREAD_COND_INITIALIZER; // condition when buffer is not empty
pthread_cond_t buffNotFull = PTHREAD_COND_INITIALIZER; // condition when buffer is not full
pthread_mutex_t outputMutex = PTHREAD_MUTEX_INITIALIZER; // mutex for output, initialized using macros, attr is NULL, error checks are not performed
pthread_mutex_t mutexa = PTHREAD_MUTEX_INITIALIZER; // mutex for synchronization
pthread_mutex_t mutexb = PTHREAD_MUTEX_INITIALIZER; // mutex for synchronization
//pthread_mutex_t counting_mutex = PTHREAD_MUTEX_INITIALIZER; // mutex for getting access to the variable cnt
pthread_mutex_t mutexc = PTHREAD_MUTEX_INITIALIZER;
bool flag = false;

int cnt = 0; // number of element currently in queue
queue<int> q; // buffer for storing values

void  *consumer(void *arg);
void *producer(void *arg);

int main() {
	srand(time(NULL));
	pthread_t producers[NUM_OF_PRODUCERS]; // array of producers thread
	pthread_t consumers[NUM_OF_CONSUMERS]; // array of consumers thread

	for (int i = 0; i < NUM_OF_PRODUCERS; i++) {
		pthread_create(&producers[i], NULL, producer, NULL);
	}

	for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
		pthread_create(&consumers[i], NULL, consumer, NULL);
	}


	for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
		pthread_join(consumers[i], NULL); // join threads
	}

	for (int i = 0; i < NUM_OF_PRODUCERS; i++) {
		pthread_join(producers[i], NULL); // join threads
	}

	return 0;
}
// 
void *consumer(void *arg) {
	for (int i = 0; i < 5; i++) {
		pthread_mutex_lock(&mutexa); // lock general mutex, so no other consumer or producer is allowed to do anything

		pthread_mutex_lock(&outputMutex); // lock mutex for output
		cout << "One of the consumers started working" << " Thread " << pthread_self() << '\n';
		pthread_mutex_unlock(&outputMutex); // unlock mutex for output
		while (cnt == 0) { // if buffer is empty
			pthread_cond_wait(&buffNotEmpty, &mutexc); // if the queue is empty, wait for at least 1 object to be pushed
			//cout << pthread_self() << '\n';
		}

		int tmp = q.front(); // the first number in the queue
		Sleep(1000 + rand() % 1000);
		pthread_mutex_lock(&outputMutex);
		cout << "Number " << tmp << " has been read" << '\t' << "Thread: " << pthread_self() << '\n';
		pthread_mutex_unlock(&outputMutex);
		q.pop(); // delete the oldest element from the queue
		cnt--; // reduce the size of the buffer


		pthread_cond_signal(&buffNotFull); // signal that buffer is not full anymore
		//cout << "76" << '\n';
		pthread_mutex_lock(&outputMutex);
		cout << "Consumer left" << '\n';
		pthread_mutex_unlock(&outputMutex);

		pthread_mutex_unlock(&mutexa); // allow another consumer or producer to come
	}
}

void *producer(void *arg) {
	for (int i = 0; i < 5; i++) {
		pthread_mutex_lock(&mutexb); // lock general mutex, so no other producer is allowed to do anything


		pthread_mutex_lock(&outputMutex);
		cout << "One of the producers started working" << " Thread " << pthread_self() << '\n';
		pthread_mutex_unlock(&outputMutex);

		while (cnt == BUFF_SIZE) { // if buffer is full
			pthread_cond_wait(&buffNotFull, &mutexc); // wait for the buffer to be not full
		}

		Sleep(1000 + rand() % 1000);
		pthread_mutex_lock(&outputMutex);
		cout << "Number " << i << " has been put" << '\t' << "Thread: " << pthread_self() << '\n';
		pthread_mutex_unlock(&outputMutex);

		q.push(i); // add another element into the buffer
		cnt++; // increase the number of elements in the buffer


		pthread_cond_signal(&buffNotEmpty); // signal that buffer is no longer empty

		pthread_mutex_lock(&outputMutex);
		cout << "Producer left" << '\n';
		pthread_mutex_unlock(&outputMutex);


		pthread_mutex_unlock(&mutexb); // allow another producer to come
	}
}