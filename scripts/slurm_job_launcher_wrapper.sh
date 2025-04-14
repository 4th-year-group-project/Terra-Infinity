#!/bin/bash
set -e

# We are going to ssh into kudu (passing through the password) and running the script there
# Get the password from the environment variable
sshpass -p $SSH_PASS ssh kudu

cd $PROJECT_ROOT

echo "Running job $1"
echo "Project Root:" $PROJECT_ROOT
sleep_duration=$((RANDOM % 10))
echo "Sleeping for $sleep_duration seconds"s
sleep $sleep_duration
sbatch $1
return_code=$?
echo "Finished running job $1"
echo "Project Root:" $PROJECT_ROOT
echo "Run Dir:" $RUN_DIR

if [ $return_code -eq 0 ]; then
    echo "Job ran successfully"
    exit 0
else
    echo "Job failed"
    exit 1
fi