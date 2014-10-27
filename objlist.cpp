#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>	
#include <cstdlib>

#include "tools.h"

using namespace std;

int main(int argc, char * argv[]){
	int show_size = 0;

	if (argc < 1) {
		cerr << "Usage:" 
			<< argv[0] 
			<<  "[-l]" 
			<< endl;
			exit(1);

	}

	if (argc == 2){
		string option = argv[1];

		if (option == "-l")
			show_size = 1;
	}


	uid_t euid = get_uid("flat_fs");

	// Set the egid to flat_fs's gid
	if(raise_privilege(euid) < 0 ){
		cerr << "Sorry there was an error accessing the repository" << endl;
		exit(1);
	}


	string user_name = get_real_username();
	string path_name = "flat_fs_repo";

	struct dirent *dir;
	DIR *d = opendir(path_name.c_str());
	char delim = '-';

	string result;

	if (d){
		while ((dir = readdir(d)) != NULL){
			struct stat st;
			string file_path = path_name + "/" + dir->d_name;
			if (stat(file_path.c_str(), &st) <0)
				cerr << "Sory error calling stat" <<endl;
		
			int size = st.st_size;

			string file_name = dir->d_name; 

			int delim_loc = file_name.find(delim);
			string files_user_name = file_name.substr(0, delim_loc);
			string object_name = file_name.erase(0, delim_loc + 1);


			if (user_name == files_user_name)
			{
				if (show_size > 0)
					cout << object_name << "\t" << size << endl;
				else 
					cout << object_name << endl;

			}

		}//end of while ((dir... ))

		closedir(d);

	}// end of if(d)

	drop_privilege(euid);


	return 0;

}
