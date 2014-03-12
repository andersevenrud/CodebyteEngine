#!/bin/bash

# Go to the root of the project (just in case)
cd ../

branchExist=false

if (( $# > 0 )); then

	for br in `git branch -l | tr '*' ' '`; do
		if [ "$br" == "$1" ]; then
			branchExist=true
		fi
	done

	if [ $branchExist == true ]; then
		if [ "$1" != "master" ]; then
			git checkout master
			git pull
			git checkout "$1"
			git rebase master
		else
			echo "Can't pull from 'master' to 'master'!"
		fi
	else
		echo "Sorry branch '$1' does not exist!"
	fi
	
else

	echo "You must pass in the name of the branch!"

fi
