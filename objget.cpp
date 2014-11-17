#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>  
#include "tools.h"
#include <string.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/md5.h>

extern "C" {
#include "crypto.h"
}


const int SIZE = 16;

using namespace std;

int main(int argc, char * argv[]){

	if (argc < 4) {
		cerr << "Usage:" 
			<< argv[0] 
			<<  " <objectname>" 
			<< " -k passphrase"
			<< endl;
		exit(1);
	}

	string pass_phrase;
	string object_name;

	// The -k can come in any order
	if (string(argv[1]).compare("-k") == 0 ){
		pass_phrase = argv[2];
		object_name = argv[3];
	}
	else if(string(argv[2]).compare("-k") == 0 ){
		pass_phrase = argv[3];
		object_name = argv[1];
	}
	else{
		cerr << "Usage:" 
			<< argv[0] 
			<<  " <objectname>" 
			<< " -k passphrase"
			<< endl;
		exit(1);
	}

	//Usernames, group names, and object names can contain letters, digits, and
	//underscores; no other characters are legal.

	if (sanitize(object_name, 0) < 0){
		cerr << "Sorry invalid input, please try again!" << endl;
		exit(1);
	}

	// Generate the MD5 sum of the pass_phrase
	// http://www.askyb.com/cpp/openssl-md5-hashing-example-in-cpp/

	unsigned char md5_digest[MD5_DIGEST_LENGTH];
	const char * pass_char = pass_phrase.c_str();

	MD5((unsigned char *)pass_char, strlen(pass_char), (unsigned char *)&md5_digest);

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

	if (file_to_read.is_open()){ 

		if (check_acl(file_to_read, user_name, group_name, "r") < 1){
			cerr << "Sorry you don't have permissions to see this file!!!" << endl;
			exit(1);
		}   

		char * iv = new char[SIZE];
		char * encrypted_key = new char[2*SIZE];

		file_to_read.read(iv, SIZE);
		file_to_read.read(encrypted_key, 2*SIZE);

		// decrypt the encrypted_key using the md5 of the password:
		char * key = new char[2*SIZE];

		aes_decrypt((unsigned char *) encrypted_key, 2*SIZE, 
					(unsigned char *) md5_digest, 
					(unsigned char *) iv, 
					(unsigned char *) key );
	
		char * encrypted_text = new char[2*SIZE];
		file_to_read.read(encrypted_text, 2*SIZE);

		while(file_to_read.gcount()>0){
			//cout << file_to_read.gcount() << endl;
			char * plain_text = new char[2*SIZE];

			int returned = aes_decrypt((unsigned char *) encrypted_text, file_to_read.gcount(), 
					(unsigned char *) key,
					(unsigned char *) iv, 
					(unsigned char *) plain_text );

			cout.write(plain_text, returned);
			file_to_read.read(encrypted_text, 2*SIZE);

			delete[] plain_text;
		}

		file_to_read.close();

		delete[] encrypted_text;
		delete[] iv;
		delete[] encrypted_key;
		delete[] key;

	}

	drop_privilege(euid);

}


