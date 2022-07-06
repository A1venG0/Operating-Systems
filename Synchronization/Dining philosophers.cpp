#include <bits/stdc++.h>
#include <pthread.h>
#include <ctime>
#include <windows.h>

#define NUM_OF_PHILOSOPHERS 5

using namespace std;

pthread_mutex_t forks[NUM_OF_PHILOSOPHERS]; // declare mutexes for every fork
pthread_mutex_t outputMutex = PTHREAD_MUTEX_INITIALIZER; // otput mutex

void *philosopher(void *arg);

int main() {

	for (int i = 0; i < NUM_OF_PHILOSOPHERS; i++) { // initialize 5 mutexes for forks
		if (pthread_mutex_init((forks + i), NULL)) { // if pthread_mutex_init is not 0, then there is an error
			cout << "pthread_mutex_init error" << '\n';
			return 1;
		}
	}

	pthread_t philosophers[NUM_OF_PHILOSOPHERS]; // declare 5 philosophers threads
	int indices[NUM_OF_PHILOSOPHERS]; // id's of forks from 0 to 4

	for (int i = 0; i < NUM_OF_PHILOSOPHERS; i++) { // initialize 5 threads for philosophers
		indices[i] = i;
		if (pthread_create((philosophers + i), NULL, &philosopher, indices + i)) { // if pthread_create is not 0, then there is an error
			cout << "pthread_create error" << '\n';
			return 1;
		}
	}

	for (int i = 0; i < NUM_OF_PHILOSOPHERS; i++) { // join philosophers threads
		if (pthread_join(philosophers[i], NULL)) { // if pthread_join is not 0, then there is an error
			cout << "pthread_join error" << '\n';
			return 1;
		}
	}

	return 0;
}

void *philosopher(void *arg) {
	int id = *(int *) arg; // philosopher's id
	int leftFork = (id + 1) % NUM_OF_PHILOSOPHERS;
	int rightFork = id % NUM_OF_PHILOSOPHERS;

	srand(time(NULL));

	for (int i = 0; i < 7; i++) {
		pthread_mutex_lock(&outputMutex);
		cout << "Philosopher " << id << " is thinking " << '\n';
		pthread_mutex_unlock(&outputMutex);

		Sleep(rand() % 1000 + 1000);

		if (!pthread_mutex_trylock(forks + leftFork)) { // try to access your left fork
			if (!pthread_mutex_trylock(forks + rightFork)) { // try to access your right fork

				pthread_mutex_lock(&outputMutex);
				cout << "Philosopher " << id << " started eating " << leftFork << ' ' << rightFork << '\n';
				pthread_mutex_unlock(&outputMutex);

				Sleep(rand() % 1000 + 1000);

				pthread_mutex_lock(&outputMutex);
				cout << "Philosopher " << id << " stopped eating " << leftFork << ' ' << rightFork << '\n';
				pthread_mutex_unlock(&outputMutex);

				pthread_mutex_unlock(forks + rightFork); // put down right fork
			}
			pthread_mutex_unlock(forks + leftFork); // put down left fork
		}
	}
	pthread_mutex_lock(&outputMutex);
	cout << "Philosopher " << id << " went home" << '\n';
	pthread_mutex_unlock(&outputMutex);
}