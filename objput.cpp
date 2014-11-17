#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>   
#include <cstdlib>
#include <string.h>

#include "tools.h"

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

	if (sanitize(object_name, 1) < 1){
		cerr << "Sorry invalid input, please try again!" << endl;
		exit(1);
	}

	uid_t euid = get_uid("flat_fs");

	// Set the egid to flat_fs's gid
	if(raise_privilege(euid) < 0 ){
		cerr << "Sorry there was an error accessing the repository" << endl;
		exit(1);
	}

	// Generate the MD5 sum of the pass_phrase
	// http://www.askyb.com/cpp/openssl-md5-hashing-example-in-cpp/

	unsigned char md5_digest[MD5_DIGEST_LENGTH];
	const char * pass_char = pass_phrase.c_str();

	MD5((unsigned char *)pass_char, strlen(pass_char), (unsigned char *)&md5_digest);

	umask(077);

	string user_name = get_real_username();
	string group_name = get_real_groupname();

	string file_name = user_name + "-" + object_name;

	string path = "flat_fs_repo/" + file_name;

	// Get the random IV
	ifstream dev_urandom;
	dev_urandom.open("/dev/urandom");

	char * iv = new char[SIZE];
	char * key = new char[SIZE];

	if (dev_urandom.is_open()){
		dev_urandom.read(iv, SIZE);
		dev_urandom.read(key, SIZE);
	}   

	unsigned char encrypted_key[2*SIZE];

	int returned_size = aes_encrypt((unsigned char *) key, SIZE, 
			(unsigned char *) md5_digest,
			(unsigned char *) iv, 
			encrypted_key);

	remove(path.c_str());
	ofstream file_to_write; 

	file_to_write.open(path.c_str(), ios::out | ios::binary | ios::app);

	if (file_to_write.is_open()){

		string line;

		string default_acl = user_name + ".* rwxpv\n%";
		file_to_write << default_acl << endl;
		file_to_write.write(iv, SIZE);
		file_to_write.write((const char *)encrypted_key, 2*SIZE);

		char * buffer = new char[SIZE];
		cin.read(buffer, SIZE);

		while (cin.gcount() > 0){
			int size_of_cipher;
			if (cin.gcount() % SIZE == 0)
				size_of_cipher = cin.gcount() + SIZE;
			else
				size_of_cipher = cin.gcount() + (SIZE - (cin.gcount() % SIZE));

			unsigned char ciphertext[size_of_cipher];

			int returned = aes_encrypt((unsigned char *) buffer, 
							cin.gcount(), 
							(unsigned char *) key,
							(unsigned char *) iv, 
							ciphertext);

			file_to_write.write((const char *)ciphertext, size_of_cipher);

			cin.read(buffer, SIZE);
		}

		delete[] buffer;
		file_to_write.close();
	}   

	delete[] key;
	delete[] iv;
	drop_privilege(euid);

}





