#!/bin/bash
FILE=$1
while IFS=" " read -r -a input; do
	user="${input[0]}"
    userdel $user
    for i in "${input[@]:1}"
    do
    	groupdel $i
    	echo "Removed group $i"
     done

done < $FILE