Flat FileSystem written in C++
==============

Security Architecture and Engineering Homework 2

This project emulates a secure flat file system built using C++. It is comprised of the following tools: 

objput name_of_file
objget name_of_file
objlist [-l]  


objsetacl name_of_file
objgetacl name_of_file>  
objtestacl name_of_file -a access_flag  

To ensure the user doesn't bypass the file system I have used setuid. All programs are run in a makefile generated username 'flat_fs'. The files in turn are stored in a makefile generated directory 'flat_fs_repo'. Each user has their own namespace and can only access another users files by using the following syntax: name_of_user+name_of_file. The user will only have access to this other users file if the correct ACL's are set using the objsetacl program. 

To install run 'make exec' which will compile and run the initialization script with the default usernames file. If you would like to use your own usernames file run 'make exec userfile="<username_file_name>"'
