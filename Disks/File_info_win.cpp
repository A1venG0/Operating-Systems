//lookupaccountsid / lookupdomensid, попробовать достать владельца файла
#include <windows.h>
#include <bits/stdc++.h>
#include <sddl.h>
#include <winbase.h>

#define DAY_NANO 864000000000
#define HOUR_NANO 36000000000
#define MIN_NANO 600000000
#define SEC_NANO 10000000

using namespace std;

bool dir(string directory);
bool printFile(const WIN32_FIND_DATAA fileInfo, const HANDLE hfile);
string toStringFileTime(FILETIME ftime);
string toStringFileAttribute(DWORD fAttr);
int fileTimeTotm(struct tm* timeInfo, FILETIME ftime);
int leapYear(unsigned short year);



int main() {

	char* path = new char[260];

	GetCurrentDirectoryA(255, path); // get the current file directory
	cout << path << '\n'; // output the current file directory
	dir(string(path));

}

bool dir(string directory) {
	WIN32_FIND_DATAA fileInfo;

	HANDLE hfile = FindFirstFileA(directory.c_str(), &fileInfo); // create a handle and call a function to find information about the first file

	if (hfile != INVALID_HANDLE_VALUE) { // if succeeded

		string buff = directory;
		buff.resize(buff.find_last_of('\\') + 1); // delete the directory name from the path in order to add it later
		cout << buff << '\n';

		do {
			if (fileInfo.cFileName != string("..") && fileInfo.cFileName != string(".")) { // debug, some files with the name . or ..
				if (printFile(fileInfo, hfile)) { // print information about the file
						dir(buff + fileInfo.cFileName + "\\*");	 // recursively call the function, if the current file is a directory
				}
			}
		} while (FindNextFileA(hfile, &fileInfo));

		FindClose(hfile); 

		return true;
	}

	return false;
}

bool printFile(const WIN32_FIND_DATAA fileInfo, const HANDLE hfile) {
	SECURITY_INFORMATION secInfo; // create variable for storing security information
	PSECURITY_DESCRIPTOR* psecDesc = new PSECURITY_DESCRIPTOR {0}; // {0} create a pointer to a buffer that receives a copy of the security descriptor
	GetFileSecurityA(fileInfo.cFileName, secInfo, psecDesc, sizeof(PSECURITY_DESCRIPTOR), NULL); // get file security

	//BY_HANDLE_FILE_INFORMATION handleFileInfo;
	//GetFileInformationByHandle(hfile, &handleFileInfo);


	//PSECURITY_DESCRIPTOR_CONTROL psecContr;
	//LPDWORD lpdwRevision;
	/*TCHAR* buff = new TCHAR[50];
	PSID_NAME_USE pes;
	long unsigned int temp = 50;
	LPDWORD buff_size = &temp;

	PSID* pOwner;
	LPBOOL lpbOwnerDefaulted;
	GetSecurityDescriptorOwner(psecDesc, pOwner, lpbOwnerDefaulted); // trying to get owner with no avail

	LookupAccountSid(NULL, &pOwner, buff, buff_size, NULL, 0, pes);

	//GetSecurityDescriptorControl(psecDesc, psecContr, lpdwRevision);


	LPSTR* stringSid;
	ConvertSidToStringSidA(pOwner, stringSid);*/

	//LPDWORD desiredAccess;
	//QuerySecurityAccessMark(secInfo, desiredAccess);

	//string owner = *stringSid;
	// LOOKupAccountSid
	cout << "File name: " << fileInfo.cFileName << '\n';
	cout << "Alternative file name(8.3 format): " << fileInfo.cAlternateFileName << '\n';
	cout << "Size: " << ( ( fileInfo.nFileSizeHigh * ( (long long) MAXDWORD + 1 ) ) + fileInfo.nFileSizeLow ) << " bytes" << '\n';
	//cout << "Owner: " << buff << '\n';
	cout << "Creation time: " << toStringFileTime(fileInfo.ftCreationTime);
	cout << "Last access time: " << toStringFileTime(fileInfo.ftLastAccessTime);
	cout << "Last modification time: " << toStringFileTime(fileInfo.ftLastWriteTime);
	cout << "Attributes: " << toStringFileAttribute(fileInfo.dwFileAttributes) << '\n';
	cout << "---------------------------------------" << '\n';

	//LocalFree(stringSid);
	if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // we have just printed a directory
		return 1; // search for the file recursively inside the newfound directory
	return 0;
}

string toStringFileTime(FILETIME ftime) {
	struct tm ti; // structure containing a calendar date and time broken down into components
	fileTimeTotm(&ti, ftime);
	char buff[26];
	asctime_s(buff, sizeof(buff), &ti); // convert tm structure into a valid string(buff)
	return string(buff);
}

int fileTimeTotm(struct tm* timeInfo, FILETIME ftime) {
	unsigned long long days;
	unsigned long long clock;
	unsigned char day_of_mon[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	days = ftime.dwHighDateTime; // specifies the high 32 bits of the ftime
	days <<= 32; // long long 64 bits, so left shift on 32 to add another 32 bits
	days |= ftime.dwLowDateTime; // specifies the low 32 bits of the ftime
	clock = days % DAY_NANO; // days modulo days in nanoseconds are nanoseconds left(days, minutes and seconds together)
	days /= DAY_NANO; // full days, without hours, minutes and seconds

	unsigned short year = 1601; 
	while(days >= (365 + leapYear(year))) { // convert days into years

		days -= (365 + leapYear(year));
		year++;
	}
	if (year < 1900) return 1; // something is wrong, tm works with years > 1900
	timeInfo -> tm_isdst = 0; // daylight saving time flag
	timeInfo -> tm_mon = 0; // month since January
	timeInfo -> tm_mday = 1; // day of the month
	timeInfo -> tm_wday = 0; // days since Sunday
	timeInfo -> tm_yday = 0; // days since January 1
	timeInfo -> tm_year = year - 1900; // years since 1900

	while(days >= day_of_mon[timeInfo->tm_mon]) { // if we have more days than in current month

		if (timeInfo -> tm_mon == 1) { // if our current month is February
			if (days >= (day_of_mon[timeInfo -> tm_mon] + leapYear(year))) { // if we have more days than in current month and if it's a leap year
				days -= (day_of_mon[timeInfo -> tm_mon] + leapYear(year)); // update the number of days accordingly
				timeInfo -> tm_mon++; // update the current month
			}
		}
		else {
			days -= day_of_mon[timeInfo -> tm_mon]; // our current month is not February, update the number of days accordingly
			timeInfo -> tm_mon++; // update the current month
		}
	}
	timeInfo -> tm_mday += days; // all the days that are left are in a current month
	timeInfo -> tm_hour = clock / HOUR_NANO; // update the number of hours since midnight
	clock %= HOUR_NANO; // only minutes and seconds are left in the clock

	timeInfo -> tm_min = clock / MIN_NANO; // update the number of minutes after the hour
	clock %= MIN_NANO; // only seconds are left

	timeInfo-> tm_sec = clock / SEC_NANO; // update the number of seconds after the minute
	clock %= SEC_NANO; // doesnt really matter

	return 0;

}

int leapYear(unsigned short year) { // leap year is every 4th year except years that are divisible by 100 and not divisible by 400

	if (year % 4 == 0) { 
		if (year % 100 == 0) {
			if (year % 400 == 0)
				return 1;
			return 0;
		}
		return 1;
	}
	return 0;
}

string toStringFileAttribute(DWORD fAttr) { // convert binary representation of file attributes into a string
	string all = "Ro HidSys[!]DirArcDevNorTmpSpsRspComOffNoICryInsVirNoSREO";
	for (int i = 1; i <= 0x00040000; i <<= 1) { // iterate through all powers of 2
		if (!(fAttr & i)) { // if the bit is not set
			all.at(log2(i) * 3) = ' '; // delete the attribute from the string (3 chars for every attribute)
			all.at(log2(i) * 3 + 1) = ' ';
			all.at(log2(i) *3 + 2) = ' ';
		}
	}
	return all; // return what is left
}

string MBFromW(LPCWSTR pwsz, UINT cp) {
    int cch = WideCharToMultiByte(cp, 0, pwsz, -1, 0, 0, NULL, NULL);

    char* psz = new char[cch];

    WideCharToMultiByte(cp, 0, pwsz, -1, psz, cch, NULL, NULL);

    string st(psz);
    delete[] psz;

   return st;
}