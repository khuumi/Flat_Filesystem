// tools.h
#include <iostream>
#include <string>
#include <fstream>

using std::string;
using std::ifstream;

int change_permissions(string filename);
int raise_privilege();
int drop_privilege();
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


