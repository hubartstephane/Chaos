#! /bin/bash

filenames=$(find . -name *.h)

for i in $filenames; do

	COUNT=0
	for j in $filenames ; do
	
		if [ $(grep --count -i $(basename $i) $j) -gt 0 ] ; then		
			COUNT=1
			#echo "on trouve " $(basename $i) "dans le fichier " $j
			break		
		fi			
	done
	
	if [ $COUNT -eq 0 ] ; then	
		echo $(basename $i) "not referenced"		
	fi
done