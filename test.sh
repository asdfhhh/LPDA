#!/bin/bash
file="test.sh"
if [ -f "$file" ]
then
	echo "$file found."
else
	echo "$file not found."
fi
