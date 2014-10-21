#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdlib>

#include "tools.h"

using namespace std;

int main(int argc, char * argv[]){

	if (argc < 2) {
		cerr << "Usage:" 
			<< argv[0] 
			<<  "[-l]\n" 
			<< endl;
			exit(1);

	}

	int show_size = 0;

	if (argc > 1){
		string option = argv[1];

		if (option == "-l")
			show_size = 1;
	}


	// Set the egid to flat_fs's gid
	if(raise_privilege() < 0 ){
		cerr << "Sorry there was an error accessing the repository" << endl;
		exit(1);
	}

	string user_name = get_real_username();

	struct dirent *dir;
	DIR *d = opendir("flat_fs_repo");
	char delim = '-';

	string result;

	if (d){
		while ((dir = readdir(d)) != NULL){
			struct stat st;
			stat(dir->d_name, &st);
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

	drop_privilege();


	return 0;

}
