#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <algorithm>  
#include <ctype.h>
#include "tools.h"

using std::ifstream;
using std::istringstream;
using std::cout;
using namespace std;

// static uid_t euid = 1003;
static uid_t ruid = getuid();

 int change_permissions(string filename)
 {
 	const char *f_name = filename.c_str();
    int stat;
 
    stat = chmod(f_name, S_IRWXU);
    if (stat)
    	cerr << "Couldn't change permissions for file " <<f_name <<endl;
    else
    	cerr << "Set permissions for " << f_name << endl;

    return 0;
 }


int get_uid(string user){
	const char *usr = user.c_str();
	struct passwd *p;
	if ((p = getpwnam(usr)) == NULL )
		return -1;

	return (int) p->pw_uid;

}


int raise_privilege(uid_t euid){
	int status;

	#ifdef _POSIX_SAVED_IDS
  		status = seteuid (euid);
	#else
  		status = setreuid (ruid, euid);
	#endif
  	if (status < 0) 
  		cerr << "Couldn't set uid." << endl;
    return status;
	
	// Set the egid to flat_fs groupid
}

int drop_privilege(uid_t euid){
  	int status;

	#ifdef _POSIX_SAVED_IDS
  		status = seteuid (ruid);
	#else
  		status = setreuid (euid, ruid);
	#endif
  	if (status < 0) 
    	cerr << "Couldn't set uid. \n" << endl;

    return status;

}

string get_real_username(){
	uid_t user_id = getuid();

	struct passwd *p;

	if ((p = getpwuid(user_id)) == NULL )
		cerr << "getpwuid() error" << endl;
	else 
		return string(p->pw_name);

	return "test";

}


string get_real_groupname(){
	uid_t group_id = getgid();

	struct group *p;
	
	if ((p = getgrgid(group_id)) == NULL )
		cerr << "getgrgid() error" << endl;
	else 
		return string(p->gr_name) ;

	return "test";
}


int valid_user(string user){

	const char *usr = user.c_str();

	if ((getpwnam(usr)) == NULL )
		return -1;
	
	return 0;
}

int valid_group(string group){

	const char *grp = group.c_str();

	if ((getgrnam(grp)) == NULL )
		return -1;
	
	return 0;
}

int validate_acl(string line){
	string allowed_ops = "rwxpv";

	try {
		string user_group, user, group, ops;

		int delim_loc = line.find(' ');

		user_group = line.substr(0, delim_loc);
		ops = line.erase(0, delim_loc + 1);

		delim_loc = user_group.find('.');

		user = user_group.substr(0, delim_loc);
		group = user_group.erase(0, delim_loc + 1);

		if (user != "*" && valid_user(user) < 0)
			return -1;

		if (group != "*" && valid_group(group) < 0)
			return -1;

		for (char &c : ops){
			if (allowed_ops.find(c) < 0)
				return -1;
		}

	}
	catch (int e){
		return -1;
	}
	
	return 0;
}

// ifstream, string username string groupname, string ops

/** Checks to see if the ACL allows for user to access **/
int check_acl(ifstream& file_to_open, 
	string user_name, 
	string group_name, 
	string access){

	string line;

	int to_return = 0;

	if (file_to_open.is_open()){
		while (getline(file_to_open, line)){
			if (line[0] == '%')
				break;

			string user_group, user, group, ops;

			int delim_loc = line.find(' ');

			user_group = line.substr(0, delim_loc);
			ops = line.erase(0, delim_loc + 1);

			delim_loc = user_group.find('.');

			user = user_group.substr(0, delim_loc);
			group = user_group.erase(0, delim_loc + 1);

			if (user_name == user || user_name == "*"){
				if(group_name == group || group == "*"){

					if(ops.find(access) > 0 )
						to_return = 1;
				}
			}
		}
	}

	return to_return;
}


//make sure that there is only letters, digits, underscores (and only one dash)
int sanitize(string to_sanitize, int type){
	if (type == 0){
		int found = to_sanitize.find_first_of("+");
		
		if (found > 0)
			to_sanitize[found] = '_';
	}

	//replace all underscores
	replace_if( to_sanitize.begin(), to_sanitize.end(), is_underscore, ' ' );

	//check for alphanumerical
	if (all_of(to_sanitize.begin(), to_sanitize.end(), ::isalnum) < 1)
		return -1;

	return 1;
}

int is_underscore(char c){

	if (c == '_')
		return true;
	else return false;
}




