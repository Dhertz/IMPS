#!/bin/bash
#Runs Tristan's auto tests and puts them in a time/dated folder. 
#Needs to be run on lab machine. 

CPROJECT=/homes/$1/$2 

DATE=$(date +%d-%m@%H:%M)

mkdir $CPROJECT/$DATE-Output

$CPROJECT/autotest/autotest $CPROJECT/IMPS/ $(git --git-dir $CPROJECT/IMPS/.git log -1 --format="%H") $CPROJECT/$DATE-Output < /dev/null >& /dev/null