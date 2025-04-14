#!/bin/bash

echo "This is running from within the cloned repository"

path=$(pwd)
echo "The current path is: $path"
git log -1

counter=0
while [ $counter -le 10 ]
do
  echo "The counter is: $counter"
  ((counter++))
done

echo "The script has finished running"