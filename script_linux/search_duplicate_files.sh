#! /usr/bin/bash

# change directory if required
if [ $# -gt 0 ] ; then
  cd $1
fi

# use read to get information line per line (instead of word per word)
while read p ; do 
  echo -n $(basename $p) "    "
  du -sh $p | words 1
done << SCRIPT
$(find . -type f)
SCRIPT
