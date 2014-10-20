#define _OPEN_SYS
#include <sys/types.h>
#include <pwd.h>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <grp.h>


#include <unistd.h>
#include <grp.h>
 #include <dirent.h>

using namespace std;

 
 void make_read_only(char * filename)
 {
    int stat;
 
    stat = chmod(filename, S_IRWXU);
    if (stat)
       printf("Couldn't make %s read-only\n", filename);
    else
       printf("Made %s read-only\n", filename);
 }




string get_real_username(){
	uid_t user_id = getuid();

	struct passwd *p;

	if ((p = getpwuid(user_id)) == NULL )
		cerr << "getpwuid() error" << endl;
	else 
		return p->pw_name ;

	return "";

}

string get_real_groupname(){
	uid_t group_id = getgid();

	struct group *g;

	if ((g = getgrgid(group_id)) == NULL )
		cerr << "getgrgid() error" << endl;
	else 
		return g->gr_name ;

	return "";
}


int raise_privilege(){

	int status;
	int egid = 1003;
	int rgid = getgid();

	#ifdef _POSIX_SAVED_IDS
  		status = setegid (egid);
	#else
  		status = setregid (rgid, egid);
	#endif
  	if (status < 0) 
    	fprintf (stderr, "Couldn't set gid.\n");
    return status;
	
	// Set the egid to flat_fs groupid
}

int valid_user(string user){

	const char *usr = user.c_str();

	if ((getpwnam(usr)) == NULL )
		return -1;
	else 
		return 0;
}

int valid_group(string group){

	const char *grp = group.c_str();

	if ((getgrnam(grp)) == NULL )
		return -1;
	else 
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
			if (allowed_ops.find(c) == -1)
				return -1;
		}

	}
	catch (int e){
		return -1;
	}
	
	return 0;
}






int main(){


	string test = "sadik.staff rwxpv";

	string test2 = "me.* rwxpv";
	string test2b = "*.me rwxpv";

	string test3 = "*.staff rwxpv";


	string test4 = "sadik staff rwxpv";


	string test5 = "sadik.staff lol";

	if (validate_acl(test) > -1)
		cout << "passed test 1" <<endl;

	if (validate_acl(test2) > -1)
		cout << "passed test 2" <<endl;
	
	if (validate_acl(test2b) > -1)
		cout << "passed test 2b" <<endl;

	if (validate_acl(test3) > -1)
		cout << "passed test 3" <<endl;

	if (validate_acl(test4) > -1)
		cout << "passed test 4" <<endl;
	
	if (validate_acl(test5) > -1)
		cout << "passed test 5" <<endl;


	
	string user_name = get_real_username();
	string group_name = get_real_groupname();

	cout << "username " << user_name << " gropname: " << group_name <<endl;


	if(raise_privilege() < 1 ){
		cerr << "Sorry there was an error accessing the repository" << endl;
	}

	cout << getegid() << endl;


	struct dirent *dir;
	DIR *d = opendir("repo");
	char delim = '-';

	string result;

	if (d){
		while ((dir = readdir(d)) != NULL){
			struct stat st;
			stat(dir->d_name, &st);
			int size = st.st_size;

			string file_name = dir->d_name; 

			cout << file_name << endl;

		}//end of while ((dir... ))

		closedir(d);

	}// end of if(d)

	cout << "cant get in" << endl;

	make_read_only("testing123");



	ofstream new_file;

	string file_name = "temp";

	string path = "flat_fs_repo/" + file_name;

	new_file.open(path);

	if (new_file.is_open()){
		new_file << "hii there" << endl;
		new_file.close();
	}
	else
		cout << "problem" << endl;





    return 0;





}