#include <windows.h>
#include <bits/stdc++.h>

#define MAX_NO_OF_CUSTOMERS 7
using namespace std;
// global variables in here
HANDLE barberCuttingEvent;
HANDLE customerReadyEvent;
HANDLE waitingRoomAccessEvent;
HANDLE outputSemaphore;
HANDLE barberSleepingEvent;
HANDLE customerBugFixEvent;
int numberOfChairs = 6;

DWORD WINAPI barber(LPVOID lpParam);
DWORD WINAPI customer(LPVOID lpParam);

int main() {
	DWORD threadId;
	HANDLE barberThread;
	HANDLE customersThreads[MAX_NO_OF_CUSTOMERS];

	int customersId[MAX_NO_OF_CUSTOMERS];
	int barberId = 0;

	// create events in here
	barberCuttingEvent = CreateEventA(NULL, false, false, NULL); // THINK ABOUT THE INITIAL STATE (3 PARAM)
	customerReadyEvent = CreateEventA(NULL, false, false, NULL); // THINK ABOUT THE INITIAL STATE (3 PARAM)
	waitingRoomAccessEvent = CreateEventA(NULL, false, true, NULL); // THINK ABOUT THE INITIAL STATE (3 PARAM)
	barberSleepingEvent = CreateEventA(NULL, false, false, NULL); // THINK ABOUT THE INITIAL STATE (3 PARAM)
	//customerBugFixEvent = CreateEventA(NULL, false, true, NULL);
	outputSemaphore = CreateSemaphore(NULL, 1, 1, NULL);


	barberThread = CreateThread(NULL, 0, &barber, &barberId, 0, &threadId); // create barber thread
	if (barberThread == NULL) {
        cout << "barberThread create error: " << GetLastError() << '\n';
        return 1;
    }

    Sleep(1300);

	for (int i = 0; i < MAX_NO_OF_CUSTOMERS; i++) {
		customersId[i] = i;
		customersThreads[i] = CreateThread(NULL, 0, &customer, &customersId[i], 0, &threadId); // create customers threads
		if (customersThreads[i] == NULL) {
            cout << "customersThreads create error: " << GetLastError() << '\n';
            return 1;
        }
	}


    WaitForMultipleObjects(MAX_NO_OF_CUSTOMERS, customersThreads, TRUE, INFINITE); // wait for all customers threads to be in signaled states
    WaitForSingleObject(barberThread, INFINITE); // wait for barber thread to be in signaled state

    for (int i = 0; i < MAX_NO_OF_CUSTOMERS; i++) { // delete customers threads
    	CloseHandle(customersThreads[i]);
    }
    CloseHandle(barberThread); // delete barber thread
    CloseHandle(barberCuttingEvent);
    CloseHandle(customerReadyEvent);
    CloseHandle(waitingRoomAccessEvent);
    CloseHandle(barberSleepingEvent);
    CloseHandle(customerBugFixEvent);
    CloseHandle(outputSemaphore);

}

DWORD WINAPI barber(LPVOID lpParam) {
	for (int i = 0; i < 50; i++) {

		WaitForSingleObject(outputSemaphore, INFINITE);
		cout << "numberOfChairs " << numberOfChairs << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL);

		if (numberOfChairs == 6) { // if nobody is inside, then sleep
			WaitForSingleObject(outputSemaphore, INFINITE);
			cout << "The barber is sleeping" << "\n";
			ReleaseSemaphore(outputSemaphore, 1, NULL);
			WaitForSingleObject(barberSleepingEvent, INFINITE); // wait for someone to wake you up
		}

		SetEvent(barberCuttingEvent); // notify other customers that barber chair is free

		WaitForSingleObject(waitingRoomAccessEvent, INFINITE); // gain access for waiting room

		numberOfChairs++;

		SetEvent(waitingRoomAccessEvent); // return access

		Sleep(1000 + rand() % 1000);

		WaitForSingleObject(outputSemaphore, INFINITE);
		cout << "The barber has finished cutting hair" << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL);

		SetEvent(customerReadyEvent); // notify customer that you have finished

	}
	return 0;
}

DWORD WINAPI customer(LPVOID lpParam) {
	int id = *static_cast<int*>(lpParam) + 1;
	for (int i = 0; i < 3; i++) {

		WaitForSingleObject(outputSemaphore, INFINITE);
		cout << "Customer " << id << " has arrived" << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL);

		WaitForSingleObject(waitingRoomAccessEvent, INFINITE); // gain acces for waiting room
		if (numberOfChairs > 0) {


			if (numberOfChairs == 6) { // if nobody is inside
				WaitForSingleObject(outputSemaphore, INFINITE);
				cout << "Customer " << id << " woke up the barber" << '\n';
				ReleaseSemaphore(outputSemaphore, 1, NULL);

				SetEvent(barberSleepingEvent); // wake up the barber
			}
			numberOfChairs--;
			SetEvent(waitingRoomAccessEvent); // return access for waiting room

			WaitForSingleObject(barberCuttingEvent, INFINITE); // make sure that barber is free and the customer before you has already left

			WaitForSingleObject(outputSemaphore, INFINITE);
			cout << "Customer " << id << " has taken the barber seat" << '\n';
			ReleaseSemaphore(outputSemaphore, 1, NULL);



			WaitForSingleObject(customerReadyEvent, INFINITE); // wait for barber to notify you that you can leave

			WaitForSingleObject(outputSemaphore, INFINITE);
			cout << "Customer " << id << " has had his hair cut and left" << '\n';
			ReleaseSemaphore(outputSemaphore, 1, NULL);

		}
		else {
			SetEvent(waitingRoomAccessEvent); // gain access for waiting room

			WaitForSingleObject(outputSemaphore, INFINITE);
			cout << "Customer " << id << " left, because there are no chairs" << '\n'; // leave, because there are no chairs left
			ReleaseSemaphore(outputSemaphore, 1, NULL);
		}
	}
	return 0;
}