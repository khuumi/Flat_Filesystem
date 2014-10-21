#!/bin/bash
FILE=$1
while IFS=" " read -r -a input; do
	user="${input[0]}"
    useradd $user
    for i in "${input[@]:1}"
    do
    	groupadd $i
    	echo "Added group $i"
    	usermod -a -G $i $user 
    	echo "Added $user to $i"
     done
done < $FILE

