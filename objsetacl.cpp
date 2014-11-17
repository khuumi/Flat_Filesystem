#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>

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
	file_to_read.open(path.c_str(), ios::in|ios::binary|ios::ate);
	streampos size;
	if (file_to_read.is_open())
		size = file_to_read.tellg();

	file_to_read.seekg(0, ios::beg);	

	// Check for this specific type of permission
	if (check_acl(file_to_read, user_name, group_name, "p") < 1){
		cerr << "Sorry you don't have permissions to set the ACL!!!" << endl;

		exit(1);
	}
	
	streampos acl_size = file_to_read.tellg();
	size = size -acl_size;
	
	char * temp_file_contents = new char[size];
	if (file_to_read.is_open())
		file_to_read.read(temp_file_contents, size);
 
	//write it to a file 
	
	file_to_read.close();
	string acl_info = "";
	
	string line; 

	//allow user input for the new ACL lines
	while (getline(cin, line)){
		if (validate_acl(line) > -1)
			acl_info = acl_info + line +"\n";
		else {
			cerr << "Bad ACL - reverting" << endl;
			drop_privilege(euid);
			exit(1);
		}	
	}

	setuid(077);
	ofstream file_to_write;
	file_to_write.open(path.c_str());

	// all acl info passed validation
	file_to_write << acl_info << "%" <<endl;

	file_to_write.write(temp_file_contents, size);

	file_to_write.close();
//	change_permissions(path);

	drop_privilege(euid);


}
