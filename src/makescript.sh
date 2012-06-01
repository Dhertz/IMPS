#!/bin/bash
#Runs Tristan's auto tests and puts them in a time/dated folder. 
#Needs to be run on lab machine. This script doesn't terminate when run automatically via ssh.
 
#Your Project directory here, needs to have the two git repos IMPS and autotest.
CPROJECT=/homes/dh611/Dropbox/FirstYearWork/Programming/cproject 

DATE=`date +%d-%m@%I:%M`

mkdir $CPROJECT/$DATE-Output

$CPROJECT/autotest/autotest /$CPROJECT/IMPS/ `git --git-dir $CPROJECT/IMPS/.git log -1 --format="%H"` $CPROJECT/$DATE-Output