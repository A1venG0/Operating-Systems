#include <bits/stdc++.h>
#include <windows.h>

#define DISK_NAME_SIZE 4

using namespace std;

void getDiskInfo();

int main() {
	getDiskInfo();
	return 0;
}

void getDiskInfo() {
	int diskNumber = GetLogicalDriveStrings(0, NULL); // returns the size of a buffer required to hold the drive strings
	TCHAR* diskNames = new TCHAR[diskNumber]; // buffer for storing drives' names
	cout << diskNumber << '\n';

 	GetLogicalDriveStrings(diskNumber, diskNames); // get information about the disks and copy it to the diskNames(buffer)

 	for (int i = 0; i < diskNumber; i++) {
 		cout << *(diskNames + i);
 	}
 	cout << '\n';

	while(*diskNames) { // buffer consists of 3 chars, followed by a null char

		TCHAR name[50]; // storing disk name
		DWORD serialNumber; // storing disk serial num
		DWORD systemFlags; // storing system flags
		TCHAR fileSystem[10]; // storing file system name
		long long freeSpace; // storing free space on a disk
		long long totalSpace; // storing total space on a disk

		lstrcpy(name, diskNames); // copy the disk name from the buffer to the name arr

		UINT DriveType = GetDriveType(name); // get drive type (values from 0 to 6 depending on the drive type)
		if (DriveType == DRIVE_FIXED) { // if this is a hard drive or flash drive

			GetVolumeInformation(name, NULL, 0, &serialNumber, 0,
				&systemFlags, fileSystem, sizeof(fileSystem)); // get information about the disk

			// get information about the free and total space
			GetDiskFreeSpaceEx(name, NULL, (PULARGE_INTEGER)&totalSpace, (PULARGE_INTEGER)&freeSpace); // PULARGE_INTEGER is a pointer to unsigned long long (64 bits)
			// second parameter is a pointer to the variable that recieves the total number of free bytes available to the user associated with the calling thread

			cout << "Disk name: " << name << '\n';
			cout << "Serial number: " << serialNumber << '\n';
			cout << "System flags: " << systemFlags << '\n';
			cout << "File system: " << fileSystem << '\n';
			cout << "Free space: " << freeSpace / (1024 * 1024) << " MB" << '\n';
			cout << "Total space: " << totalSpace / (1024 * 1024) << " MB" << '\n';
			cout << "------------------------------------------------" << '\n';
		}
		while(*diskNames) diskNames++; // in order to go to the next drive, interate through string till u find null char
		diskNames++; // when you found null char, go to the next char, because it might be a beginning of a new disk name
	}
}
