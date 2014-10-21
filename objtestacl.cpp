#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#include "tools.h"
using namespace std;

int sanitize_args(string access, string object_name);

int main(int argc, char * argv[]){

	if (argc < 4) {
		cerr << "Usage:" 
			<< argv[0] 
			<<  "-a <access> <objectname>\n" 
			<< endl;
	}

	string access = argv[2];
	string object_name = argv[3];

	//Usernames, group names, and object names can contain letters, digits, and
	//underscores; no other characters are legal.

	if (sanitize_args(access, object_name) < 1){
		cerr << "Sorry invalid input, please try again!\n" << endl;
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

	string path = "flat_fs_repo/" + file_name;


	ifstream file_to_open;
	file_to_open.open(path.c_str());

	// Check for this specific type of permission
	if (check_acl(file_to_open, user_name, group_name, access) < 1)
		cout << "denied" << endl;
	else 
		cout << "allowed" << endl;

	file_to_open.close();
	drop_privilege(euid);

	return 0;
}

int sanitize_args(string access, string object_name){
	if (access.find_first_not_of("pvrwxp") >= 0)
		return -1;
	else 
		return sanitize(object_name, 0);

	return 1;
}

