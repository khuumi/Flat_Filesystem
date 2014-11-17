Flat FileSystem written in C++
==============
Password for tinyvm = hogbacon
Password for user = eatbacon

Security Architecture and Engineering Homework 3

This project emulates a secure flat file system built using C++.  It is comprised of the following tools: 

objput name_of_file -k pass_phrase
objget name_of_file -k pass_phrase
objlist [-l]  


objsetacl name_of_file  
objgetacl name_of_file> 
objtestacl name_of_file -a access_flag  

To ensure the user doesn't bypass the file system I have used setuid. All programs are run in a makefile generated username 'flat_fs'. The files in turn are stored in a makefile generated directory 'flat_fs_repo'. Each user has their own namespace and can only access another users files by using the following syntax: name_of_user+name_of_file. The user will only have access to this other users file if the correct ACL's are set using the objsetacl program. 

I have also implemented 128-bit AES encryption using the CBC encryption library. I get an MD5 hash of the password, then generate a random key and a random IV and use that to encrypt the file contents. I then encrypt the key using the MD5 of the passphrase and store that encrypted key in the file alongside the IV. By putting both these in the header of the file I create another level of obscurity, as anyone who gains access to one of these files will see many bytes but will not know which bytes belong to the key or the IV. (Unless of course they have access to the source code)


To install run 'sudo make exec' which will compile and run the initialization script with the default usernames file. If you would like to use your own usernames file run 'sudo make exec userfile="username_file_name"'

For tests run: sudo make test


