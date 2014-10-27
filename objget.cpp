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
			<<  " <objectname>" 
			<< endl;
			exit(1);

	}

	string object_name = argv[1];

	//Usernames, group names, and object names can contain letters, digits, and
	//underscores; no other characters are legal.

	if (sanitize(object_name, 0) < 0){
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

	int delim_loc = object_name.find('+');
	string owner_name = user_name;

	if ( delim_loc > 0){
		owner_name = object_name.substr(0, delim_loc);
		object_name = object_name.erase(0, delim_loc + 1);
	} 

	string file_name = owner_name + "-" + object_name;

	string path = "flat_fs_repo/" + file_name;

	ifstream file_to_read (path.c_str(), ios::in|ios::binary);

	if (check_acl(file_to_read, user_name, group_name, "r") < 1){
		cerr << "Sorry you don't have permissions to see this file!!!" << endl;
		exit(1);
	}	
	

	char * buffer = new char[1024];

	if (file_to_read.is_open()){
		file_to_read.read(buffer, 1024);
		while(file_to_read.gcount()>0){
			cout.write(buffer, file_to_read.gcount());
			file_to_read.read(buffer, 1024);
		}
	}

		
	
	file_to_read.close();
		
	delete[] buffer;
	
	drop_privilege(euid);

}


