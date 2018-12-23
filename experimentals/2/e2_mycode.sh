#!/bin/bash 

echo -n "Enter your string: "
read input;
echo -n "$input" > f1;
echo -n "Enter the character you want to find and replace: ";
read charBeReplaced;
echo -n "Enter the character that will be used for override: ";
read charNew;
echo -n "Enter the number of repetitions: ";
read repetition;
echo -n "Enter the time of replacement: ";
read replacement_time;
ls -l f1;
od -c f1;
gcc e2_mycode.c -o e2_mycode;
./e2_mycode $charBeReplaced $charNew $repetition $replacement_time;
ls -l f1;
od -c f1;

#echo "Show log:";
#cat log;
