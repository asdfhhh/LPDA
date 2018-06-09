#!/bin/sh
#read the binary to L1 root
PREFIX=/wns/run03/LPDA201805/
DATA=data/
run=10000
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
	if [ -f "$run/$Output" ]
	then echo "$LINE exists, skip..."
	else ./decode $LINE $run/$Output
	fi
done
echo "Decode DONE!"
rm -f .$run
echo "The ROOT of $run is processing..."
ls $run/daq*$run* > .$run
cat .$run |while read LINE
do
        echo "Analysis Data file:$LINE..."
        ./pickup $LINE
done
echo "Analysis DONE!"
rm -f .$run $run.out

