#!/bin/sh
#read the L1 root to L2
echo "Please input the run No:"
read run 
echo "The ROOT of $run is processing..."
ls $run/daq*$run* > .$run
cat .$run |while read LINE
do
	echo "Analysis Data file:$LINE..."
	./pickup $LINE
done
echo "Analysis DONE!"
rm -f .$run
