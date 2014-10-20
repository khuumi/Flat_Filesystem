// tools.h
#include <iostream>
#include <string>
#include <fstream>

using std::string;

int change_permissions(string filename);
int raise_privilege();
int drop_privilege();
string get_real_username();
int get_real_groupname();
int valid_user(string user);
int valid_group(string group);
int validate_acl(string line);
int sanitize(string to_sanitize, int type);
int is_underscore(char c);



