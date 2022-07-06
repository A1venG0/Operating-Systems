#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <errno.h>

using namespace std;

void find_dir(string path);

int main() {

	find_dir("/home");
	return 0;

}

void find_dir(string path) {
	DIR* dirr; // pointer to the data type that represents a directory stream

	map<int, pair<int, vector<string>>> mp;

	struct dirent* diren; // structure for storing file serial number, file name and file type
	struct stat attr; // structure for storing file information (inode number, type, number of hard links, user ID, group ID, total size)
			// last access time, last modification time, last status change time

	if ((dirr = opendir(path.c_str())) == NULL) {
		return;
	}

	while ((diren = readdir(dirr)) != NULL) {
		if (diren -> d_type == DT_DIR) { // if current file type is a directory
			if (strcmp(diren -> d_name, ".") == 0 || strcmp(diren -> d_name, "..") == 0) {
				continue;
			}
			string add_path = path + "/" + string(diren -> d_name); // add to the path this directory and recursively find hard links in it
			find_dir(add_path);

		}
		else {
			string glob_path = path + "/" + string(diren -> d_name); // current file is not a directory
			if (stat(glob_path.c_str(), &attr) < 0) {
				cout << errno << ' ' << glob_path << '\n';
			}
			else {
				if (attr.st_nlink > (uint) 1) {
					mp[attr.st_ino].first = attr.st_nlink;
					mp[attr.st_ino].second.push_back(glob_path);
				}
			}
		}
	}
	map<int, pair<int, vector<string>>>::iterator itr;
	for (itr = mp.begin(); itr != mp.end(); itr++) {
		cout << "File " << itr->second.second[0] << " has " << itr->second.first << " hardlinks" << '\n';
		for (int i = 1; i < itr->second.second.size(); i++) {
			cout << itr -> second.second[i] << '\n';
		}
	}
	closedir(dirr); // close the directory stream
}