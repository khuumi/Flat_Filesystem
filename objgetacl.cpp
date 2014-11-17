#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>	

#include "tools.h"

using namespace std;

int main(int argc, char * argv[]){

	if (argc < 2) {
		cerr << "Usage:" 
			<< argv[0] 
			<<  "<objectname>\n" 
			<< endl;
			exit(1);

	}

	string object_name = argv[1];

	//Usernames, group names, and object names can contain letters, digits, and
	//underscores; no other characters are legal.

	if (sanitize(object_name, 0) < 1){
		cerr << "Sorry invalid input, please try again!" << endl;
		exit(1);
	}

	uid_t euid = get_uid("flat_fs");

	// Set the egid to flat_fs's gid
	if(raise_privilege(euid) < 0 ){
		cerr << "Sorry there was an error accessing the repository" << endl;
		exit(1);
	}

	string user_name = get_real_username();
	string group_name = get_real_groupname();

	string owner_name = user_name;

	int delim_loc = object_name.find('+');


	if ( delim_loc > 0){
		owner_name = object_name.substr(0, delim_loc);
		object_name = object_name.erase(0, delim_loc + 1);
	}

	string file_name = owner_name + "-" + object_name;
	string path = "/flat_fs_repo/" + file_name;


	ifstream file_to_read;
	file_to_read.open(path.c_str());

	// Check for this specific type of permission
	if (check_acl(file_to_read, user_name, group_name, "v") < 1){
		cerr << "Sorry you don't have permissions to see the ACL!!!" << endl;
		exit(1);

	}

	string line;

	file_to_read.seekg(0);

	if (file_to_read.is_open()){
		while (getline(file_to_read, line)){
			if (line == "%")
				break;
			cout << line << "\n";

		}
		file_to_read.close();
	}

	drop_privilege(euid);

}
