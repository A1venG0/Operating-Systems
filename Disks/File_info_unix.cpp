#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <pwd.h>
#include <grp.h>
#include <filesystem>

using namespace std;

void find_directory(string path);

int main() {
	string directory;
	cin >> directory; // input a directory

	find_dir(directory);
	return 0;
}

void find_directory(string path) {
	DIR* dirr; 

	struct dirent* ent; // structure for storing file serial number and file name

	if ((dirr = opendir(path.c_str())) != NULL) { // open a directory stream, returns a pointer to the directory stream
		while((ent = readdir(dirr)) != NULL) { // returns NULL on reaching the end of the directory stream
			if (ent -> d_type == DT_DIR) {
				if (strcmp(ent -> d_name, ".") == 0 || strcmp(ent -> d_name, "..") == 0)
					continue;
				find_dir(path + "/" + ent -> d_name); // if the current file is a directory, recursively find all the files inside the directory
			}

			struct stat attr; // structure for storing file information (inode number, type, number of hard links, user ID, group ID, total size)
			// last access time, last modification time, last status change time

			if (stat(path.c_str(), &attr) < 0) {
				cout << "Error occured" << '\n';
			}
			else {
				cout << "File name: " << ent -> d_name << '\n';
				cout << "File size: " << attr.st_size << " bytes " << '\n';
				cout << "Last access time: " << asctime(gmtime(&attr.st_atime)); // gmtime converts the calendar time to broken-down time representation in UTC, then asctime converts it to a string
				cout << "Last modification time: " << asctime(gmtime(&attr.st_mtime));
				cout << "Last status change: " << asctime(gmtime(&attr.st_ctime));
				cout << "File type: ";
				if (ent->d_type == DT_BLK) {
					cout << "block device" << '\n';
				}
				else if (ent->d_type == DT_CHR) {
					cout << "character device" << '\n';
				}
				else if (ent->d_type == DT_DIR) {
					cout << "directory" << '\n';
				}
				else if (ent->d_type == DT_FIFO) {
					cout << "named pipe (FIFO)" << '\n';
				}
				else if (ent->d_type == DT_LNK) {
					cout << "symbolic link" << '\n';
				}
				else if (ent->d_type == DT_REG) {
					cout << "regular file" << '\n';
				}
				else if (ent->d_type == DT_SOCK) {
					cout << "UNIX domain socket" << '\n';
				} 
				else {
					cout << "Unknown" << '\n';
				}



				string permissions = "";
				if (ent->d_type == DT_DIR) {
					permissions += "d";
				}
				else {
					permissions += "-";
				}

				mode_t permis = attr.st_mode;

				permissions += (permis & S_IRUSR) ? "r" : "-";
				permissions += (permis & S_IWUSR) ? "w" : "-";
				permissions += (permis & S_IXUSR) ? "x" : "-";
				permissions += (permis & S_IRGRP) ? "r" : "-";
				permissions += (permis & S_IWGRP) ? "w" : "-";
				permissions += (permis & S_IXGRP) ? "x" : "-";
				permissions += (permis & S_IROTH) ? "r" : "-";
				permissions += (permis & S_IWOTH) ? "w" : "-";
				permissions += (permis & S_IXOTH) ? "x" : "-";

				struct passwd *pw = getpwuid(attr.st_uid); // getting owner by id
				struct group *gr = getgrgid(attr.st_gid); // getting group by id

				cout << "File permission: " << permissions << '\n';

				cout << "Owner ID: " << attr.st_uid << '\n'; // user ID of the creating process
				cout << "Group ID: " << attr.st_gid << '\n'; // group ID of the creating process/ parent directory
				cout << "Owner: " << pw -> pw_name << '\n';
				cout << "Group: " << gr -> gr_name << '\n';

				cout << "----------------------------" << '\n';
			}
		}
		closedir(dirr); // close the directory stream
	}
}