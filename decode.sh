#!/bin/sh
#read the binary to L1 root
PREFIX=/wns/run03/LPDA201805/
DATA=data/
echo "Please input the run No:"
read run
#run=10936
echo "The data of $run is processing..."
if [ ! -d "$run" ]; then
  mkdir $run
fi
ls $PREFIX*$run*>.$run
cat .$run |while read LINE
do
	echo "Decode file:$LINE..."
	Output=$(echo ${LINE/$PREFIX})
	Output=$(echo ${Output/raw/root})
	./decode $LINE $run/$Output
done
echo "Decode DONE!"
rm -f .$run
