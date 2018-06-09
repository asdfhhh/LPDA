#!/bin/sh
echo "Please input the begin run No:"
read bn
echo "Please input the end run No:"
read en
for (( c=$bn; c<=$en; c++ ))
do  
   sed "s/10000/$c/g" model.sh >$c.sh
   chmod 777 $c.sh 
   nohup ./$c.sh >$c.out&
done
