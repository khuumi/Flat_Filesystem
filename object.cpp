#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char * argv[]){

	if (argc < 6) {
		cerr << "Usage:" 
			<< argv[0] 
			<<  "-u <username> -g <groupname> <objectname>\n" 
			<< endl
	}

	string user_name = owner_name = argv[2];
	string group_name = argv[4];
	string object_name = argv[5]

	//Usernames, group names, and object names can contain letters, digits, and
	//underscores; no other characters are legal.

	if (sanitize_args(user_name, group_name, object_name) < 1){
		cerr << "Sorry invalid input, please try again!\n" << endl
		exit(1);
	}

	if (valid_user_name(user_name, group_name) < 1){
		cerr << "Sorry invalid credentials, please try again!\n" << endl;
		exit(1);
	}

	int delim_loc = object_name.find('+');
	if ( delim_loc > 0){
		owner_name = object_name.substr(0, delim_loc);
		object_name = objec_name.erase(0, delim_loc + 1);
	}

	string file_name = owner_name + "-" + object_name;

	ifstream file_to_read;

	if (check_acl(file_to_read, user_name, group_name) < 1){
		cerr << "Sorry you don't have permissions to see this file!!!\n" << endl
	}

	string line;
	file_to_read.open(file_name);

	if (file_to_read.is_open()){
		while (getline(file_to_read, line))
			cout << line << "\n";

		file_to_read.close();
	}
}


int is_underscore(char c){

	if c == '_'
		return true;
	else return false;
}

int sanitize_args(string user_name, string group_name, string object_name){
	if (sanitize(user_name) < 0)
		return -1;
	else if (sanitize(group_name) < 0)
		return -1;
	else {
		//Make sure there is only one '+'
		found = object_name.find_first_of("+");
		if (found > 0)
			object_name.replace(found);

		return sanitize(object_name);
	}

	return 1;
}


//make sure that there is only letters, digits, underscores (and only one dash)
int sanitize(string to_sanitize)){
	//replace all underscores
	replace_if( object_name.begin(), object_name.end(), is_underscore, ' ' );

	//check for alphanumerical
	if (all_of(object_name.begin(), object_name.end(), ::isalnum) < 1)
		return -1;

	return 1;
}


int valid_credentials(string user_name, string group_name){
	return 1;
}





//Here we make the assumption that the proxy file has not been tampered
//with and that the variables are corrcect
// int valid_credentials(string user_name, string group_name ){

// 	string proxy; 
// 	ifstream users_proxy;

// 	users_proxy.open("userfile.proxy");

// 	if (users_proxy.is_open())
// 		getline(users_proxy, proxy);

// 	users_proxy.close();

// 	ifstream users_file; 
// 	users_file.open(proxy)
// 	string line;
// 	if (users_file.is_open()){
// 		while (getline(users_file, line)){

// 			int delim_loc = object_name.find(' ');
// 			if ( delim_loc > 0){
// 				user_in_file = object_name.substr(0, delim_loc);
// 				if ( user_in_file == user_name)
// 					// Check the rest 

// 				rest_of_word = objec_name.substr(0, delim_loc + 1);



// 				}





			// if last is a new line then change it to NULL 

			// delim the line by space first

			//first is username, second is group name

			//if username matches then check group to make sure that a group matches






	// 	}



	// }



	//open the user proxy

	//do a gets
	// if last character is new line change to null

	//open up that file







}




