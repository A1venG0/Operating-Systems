#include <windows.h>
#include <bits/stdc++.h>

#define MAX_NO_OF_READERS 5
#define MAX_NO_OF_WRITERS 3
using namespace std;

int readCount = 0, writeCount = 0, temp = 5;
HANDLE readSemaphore;
HANDLE writeSemaphore;
HANDLE outputSemaphore;

DWORD WINAPI reader(LPVOID lpParam);
DWORD WINAPI writer(LPVOID lpParam);
bool flag = false;

int main() {
	DWORD threadId;
	HANDLE readersThreads[MAX_NO_OF_READERS];
	HANDLE writersThreads[MAX_NO_OF_WRITERS];


	int readersId[MAX_NO_OF_READERS];
	int writersId[MAX_NO_OF_WRITERS];

	readSemaphore = CreateSemaphore(NULL, 1, 1, NULL); // LOOK HERE second param
	writeSemaphore = CreateSemaphore(NULL, 1, 1, NULL); // LOOK HERE second param
	outputSemaphore = CreateSemaphore(NULL, 1, 1, NULL);


	if (readSemaphore == NULL) {
        cout << "readSemaphore create error: " << GetLastError() << '\n';
        return 1;
    }
    if (writeSemaphore == NULL) {
        cout << "writeSemaphore create error: " << GetLastError() << '\n';
        return 1;
    }

    for (int i = 0; i < MAX_NO_OF_WRITERS; i++) {
		writersId[i] = i;
		writersThreads[i] = CreateThread(NULL, 0, &writer, &writersId[i], 0, &threadId);
		if (writersThreads[i] == NULL) {
            cout << "writersThreads create error: " << GetLastError() << '\n';
            return 1;
        }
	}
	Sleep(1300);
	for (int i = 0; i < MAX_NO_OF_READERS; i++) {
		readersId[i] = i;
		readersThreads[i] = CreateThread(NULL, 0, &reader, &readersId[i], 0, &threadId);
		if (readersThreads[i] == NULL) {
            cout << "readersThreads create error: " << GetLastError() << '\n';
            return 1;
        }
	}

	WaitForMultipleObjects(MAX_NO_OF_READERS, readersThreads, TRUE, INFINITE);
	WaitForMultipleObjects(MAX_NO_OF_WRITERS, writersThreads, TRUE, INFINITE);
	for (int i = 0; i < MAX_NO_OF_READERS; i++) { // deleting threads
		CloseHandle(readersThreads[i]);
	}
	for (int i = 0; i < MAX_NO_OF_WRITERS; i++) { // deleting threads
		CloseHandle(writersThreads[i]);
	}

	CloseHandle(readSemaphore); // deleting semaphore
	CloseHandle(writeSemaphore); // deleting semaphore
	CloseHandle(outputSemaphore); // deleting semaphore
	
	return 0;
}

DWORD WINAPI reader(LPVOID lpParam) {

	int id = *static_cast<int*>(lpParam) + 1;
	for (int i = 0; i < 10; i++) {
		if (flag) {
			Sleep(1000);
			continue;
		}
		WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
		cout << "reader " << id << " came to the queue" << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output

		WaitForSingleObject(readSemaphore, INFINITE); // gain access for reading
		readCount++;

		if (readCount == 1) {

			WaitForSingleObject(writeSemaphore, INFINITE); // prevent all writers from writing
			WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
			cout << "The first reader came and locked database" << '\n';
			ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output
		}

		ReleaseSemaphore(readSemaphore, 1, NULL); // let someone else gain access for reading

		WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
		cout << "reader " << id << " reading " << temp << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output
		Sleep(1000 + rand() % 1000);


		WaitForSingleObject(readSemaphore, INFINITE); // gain access for leaving reading
		readCount--;
		WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
		cout << "reader " << id << " left" << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output


		if (readCount == 0) { // if reader was the last one
			WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
			cout << "reader " << id << " went home and opened database" << '\n';
			ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output

			ReleaseSemaphore(writeSemaphore, 1, NULL); // allow writers to write
		}
		ReleaseSemaphore(readSemaphore, 1, NULL); // let someone else gain access for reading/leaving

		Sleep(3000 + rand() % 3000);
	}
	return 0;
}

DWORD WINAPI writer(LPVOID lpParam) {
	int id = *static_cast<int*>(lpParam) + 1;
	for (int i = 0; i < 10; i++) {

		WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
		cout << "writer " << id << " came to the queue" << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output
		flag = true;
		WaitForSingleObject(writeSemaphore, INFINITE); // gain access for writing
		flag = false;

		WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
		cout << "writer " << id << " is writing" << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output
		temp += 5;
		Sleep(1000); // writing
		

		WaitForSingleObject(outputSemaphore, INFINITE); // gain access for output
		cout << "writer " << id << " left" << '\n';
		ReleaseSemaphore(outputSemaphore, 1, NULL); // let someone else output

		ReleaseSemaphore(writeSemaphore, 1, NULL); // let someone else write

	}
	return 0;
}