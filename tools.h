// tools.h
#include <iostream>
#include <string>
#include <fstream>

using std::string;
using std::ifstream;

int change_permissions(string filename);
int get_uid(string user);
int raise_privilege(uid_t euid);
int drop_privilege(uid_t euid);
string get_real_username();
string get_real_groupname();
int valid_user(string user);
int valid_group(string group);
int validate_acl(string line);
int sanitize(string to_sanitize, int type);
int is_underscore(char c);
int check_acl(ifstream& file_to_open, 
	string user_name, 
	string group_name, 
	string access);


