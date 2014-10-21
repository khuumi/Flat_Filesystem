#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>	
#include <cstdlib>

#include "tools.h"
using namespace std;

int main(int argc, char * argv[]){

	if (argc < 2) {
		cerr << "Usage:" 
			<< argv[0] 
			<<  " <objectname>\n" 
			<< endl;
			exit(1);
	}

	string object_name = argv[1];

	//Usernames, group names, and object names can contain letters, digits, and
	//underscores; no other characters are legal.

	if (sanitize(object_name, 0) < 1){
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
	if (check_acl(file_to_open, user_name, group_name, "p") < 1){
		cerr << "Sorry you don't have permissions to set the ACL!!!\n" << endl;
		exit(1);
	}

	string line;

	// Go back to the beginning of the ACL
	file_to_open.seekg(0);
	string old_acl = "";
	if (file_to_open.is_open()){

		while (getline(file_to_open, line)){
			if (line[0] == '%')
				break;
			old_acl = line + '\n';
		}
	}	

	/** First create a temp file to copy to **/
	fstream temp_file; 
	string path_to_temp = "flat_fs_repo/TEMP";
	temp_file.open(path_to_temp.c_str());

	// copy (starting from after the ACL)
	if ( temp_file.is_open() && file_to_open.is_open()){
		while(getline(file_to_open, line))  
			temp_file << line << endl;
	}

	// Close up the temp file and then reopen it as an input stream, close up the previous input
	temp_file.close();
	// change_permissions(path_to_temp);

	temp_file.open(path_to_temp.c_str());

	file_to_open.close();

	ofstream file_to_write;
	file_to_write.open(path.c_str());

	//allow user input for the new ACL lines
	while (getline(cin, line)){

		if (validate_acl(line) > -1)
			file_to_write << line << endl;
		else {
			file_to_write << old_acl << endl;
			cerr << "Bad ACL - reverting" << endl;

			break;
		}
		
	}

	//input the final delimiter 
	file_to_write <<"%\n" << endl;

	while (getline(temp_file, line))
		file_to_write << line << endl;

	temp_file.close();
	file_to_write.close();
	change_permissions(path);

	remove(path_to_temp.c_str());
	drop_privilege(euid);


}
