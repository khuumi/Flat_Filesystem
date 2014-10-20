// tools.h
#include <iostream>
#include <string>
#include <fstream>

void change_permissions(string filename);
int raise_privilege();
int drop_privilege();
string get_real_username();
int get_real_groupname();
int valid_user(string user);
int valid_group(string group);
int validate_acl(string line);


