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
    char md5_result[33];

    for(int i = 0; i < 16; i++)
        sprintf(&md5_result[i*2], "%02x", (unsigned int)md5_digest[i]);

    printf("md5 digest: %s\n", md5_result);



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

        char * iv = new char[16];
        char * encrypted_key = new char[32];

        file_to_read.read(iv,16);
        file_to_read.read(encrypted_key, 32);

        // decrypt the encrypted_key using the md5 of the password:

        char * key = new char[32];

        if (aes_decrypt((unsigned char *) encrypted_key, 32, 
                        (unsigned char *) md5_result, 
                        (unsigned char *) iv, (unsigned char *) key ) != 32){
            cerr << "Sorry decryption failed" <<endl;
            exit(1);
        }


        char * buffer = new char[1024];

        file_to_read.read(buffer, 1024);

        while(file_to_read.gcount()>0){

            //decrpyt buffer 

            char * text = new char[file_to_read.gcount()];

            if (aes_decrypt((unsigned char *) buffer, file_to_read.gcount(), 
                        (unsigned char *) key, (unsigned char *) iv, 
                        (unsigned char *)text ) != file_to_read.gcount()) {
                cerr << "Sorry decryption failed" <<endl;
                exit(1);
            }

            text[file_to_read.gcount()] = '\0';

            cout.write(buffer, file_to_read.gcount());
            file_to_read.read(buffer, 1024);
        }

        file_to_read.close();

        delete[] buffer;
        delete[] iv;
        delete[] encrypted_key;
        delete[] key;

    }

 

    drop_privilege(euid);

}


