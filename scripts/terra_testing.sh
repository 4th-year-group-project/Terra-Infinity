#!/bin/bash -i

# This is a bash wrapper script for the terraform testing infrastructure
Help()
{
    # Display Help
    echo "This script is a wrapper for the terraform testing infrastructure"
    echo
    echo "Syntax: ./scripts/terra_testing.sh [-r|--regression-type <Regression Type>] [(-b|--branch <Branch Name> -n|--number-commits <Number of Commits>) | (-A|--commit-after <Commit Hash After> | -B|--commit-before <Commit Hash Before>)] [-h|--help|-v|--verbose|-a|--async]"
    echo "options:"
    echo "-r  | --regression-type           Provide the regression type"
    echo "-B  | --commit-before             Provide the comm>t hash. This option is only provided alongside -a"
    echo "-b  | --branch                    Provide the branch name"
    echo "-n  | --number-commits            Provide the number of commits to compare to. This option is only provided alongside -b"
    echo "-A  | --commit-after              Provide the commit hash after. This option is only provided alongside -b"
    echo "-h  | --help                      Print this Help."
    echo "-v  | --verbose                   Print the output of the terraform testing script"
    echo "-a  | --async                     Run the terra testing testbenches script asynchronously"
    echo "-u  | --update                    Update the terraform testing infrastructure"
    echo
}

#################################################
# Main script
#################################################
set -e
# Default arguments values
NUMBER_COMMITS=5

PARSED_ARGUMENTS=$(getopt -a -n terra_testing -o r:B:b:n:A:hvau --long regression-type:,commit-before:,branch:,nummber-commits:,commit-after:,help,verbose,async -- "$@")
VALID_ARGUMENTS=$?

if [ "$VALID_ARGUMENTS" != "0" ]; then
    Help
    exit 1
fi

echo "PARSED_ARGUMENTS: $PARSED_ARGUMENTS"
eval set -- "$PARSED_ARGUMENTS"
while :
do
    case "$1" in
        -a | --async) ASYNC="-a" ; shift ;;
        -v | --verbose) VERBOSE="-v" ; shift ;;
        -u | --update) UPDATE="-u" ; shift ;;
        -r | --regression-type) REGRESSION_TYPE="$2" ; shift 2 ;;
        -B | --commit-before) COMMIT_BEFORE="$2" ; shift 2 ;;
        -b | --branch) BRANCH_NAME="$2" ; shift 2 ;;
        -n | --number-commits) NUMBER_COMMITS="$2" ; shift 2 ;;
        -A | --commit-after) COMMIT_AFTER="$2" ; shift 2 ;;
        -h | --help) Help ; exit 0 ;;
        # -- means the end of the arguments; drop this, and break out of the while loop
        --) shift ; break ;;
        # If invalid options are passed, then getopt should have reported an error, which we checked as VALID_ARGUMENTS
        *) echo "Unexpected option: $1 - this should not happen." ; Help ; exit 1 ;;
    esac
done

# Output the arguments values
if [[ $VERBOSE ]]; then
    echo "Regression Type: $REGRESSION_TYPE"
    echo "Branch Name: $BRANCH_NAME"
    echo "Number of Commits: $NUMBER_COMMITS"
    echo "Commit Before: $COMMIT_BEFORE"
    echo "Commit After: $COMMIT_AFTER"
    echo "Async: $ASYNC"
    echo "Verbose: $VERBOSE"
    echo "Update: $UPDATE"
    echo "Remaining arguments: $@"
fi

# A random git command we might need for future PR implementations
# git --no-pager diff --name-only ef8e9baf811321d4d04b2d2e746c9d83aeea1c7e $(git merge-base ef8e9baf811321d4d04b2d2e746c9d83aeea1c7e infra/python)

# We need to check that the current directory is the root of the repository
if [[ ! -d .git ]]; then
    echo "This script must be run from the root of the repository"
    exit 1
fi

# Ensure that a regression type is provided
if [[ -z $REGRESSION_TYPE ]] ; then
    echo "Error - A regression type must be provided"
    Help
    exit 1
fi

if [[ -z $BRANCH_NAME ]] &&  [[ -z $COMMIT_BEFORE || -z $COMMIT_AFTER ]]; then
    echo "Error - A branch name or commit hash before and after must be provided"
    Help
    exit 1
elif [[  ! -z $COMMIT_BEFORE && -z $COMMIT_AFTER ]]; then
    echo "Error - If a commit hash before is provided then a commit hash after must be provided"
    Help
    exit 1
elif [[  -z $COMMIT_BEFORE && ! -z $COMMIT_AFTER ]]; then
    echo "Error - If a commit hash after is provided then a commit hash before must be provided"
    Help
    exit 1
fi
if [[ -z $BRANCH_NAME ]] && [[ $NUMBER_COMMITS != 5 ]]; then
    echo "Error - You cannot provide a number of commits without a branch name"
    Help
    exit 1
fi

if [[ ! -z $BRANCH_NAME ]] && [[ ! -z $COMMIT_BEFORE || ! -z $COMMIT_AFTER ]]; then
    echo "Error - You cannot provide a branch name and a commit hash before and after"
    Help
    exit 1
fi

# Check that NUMBER_COMMITS is a number
if ! [[ $NUMBER_COMMITS =~ ^[0-9]+$ ]] ; then
    echo "Error - Number of commits must be a number"
    Help
    exit 1
fi

# We need to set up the environment
# source ~/.bashrc

if [[ -z $PROJECT_ROOT ]]; then
    echo "Warning - The PROJECT_ROOT environment variable is not set"
    echo "sourcing from current directory"
    source ./sourceme --testing
else
    source $PROJECT_ROOT/sourceme --testing
fi

# We need to check if $3 is --branch and if so, we need to check if $4 is a valid branch
if [[ $BRANCH_NAME ]]; then
    git rev-parse --verify $BRANCH_NAME > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Error - The branch name provided is not valid"
        exit 1
    fi
fi

# Make the regression run directory with the format "Run-<Regression Type>-<date>"
# export RUN_DIR=$PROJECT_ROOT/../Run-$REGRESSION_TYPE-$(date +"%Y-%m-%d-%H-%M-%S")
export RUN_DIR=/dcs/large/efogahlewem/Testing_Jobs/Run-$REGRESSION_TYPE-$(date +"%Y-%m-%d-%H-%M-%S")
echo $RUN_DIR
mkdir $RUN_DIR

# Make sure the ./scripts/generated_scripts directory exists
mkdir -p $PROJECT_ROOT/scripts/generated_scripts

if [[ $BRANCH_NAME ]]; then
    current_commit=$(git rev-parse $BRANCH_NAME)
    previous_commit=$(git rev-parse $BRANCH_NAME~$NUMBER_COMMITS)
    if [[ $VERBOSE ]]; then
        echo "Current commit: $current_commit"
        echo "Previous commit: $previous_commit"
    fi
    git diff --name-only $previous_commit $current_commit > $RUN_DIR/changed_files.txt
    echo "python ./infrastructure/src/python/terra_testing/main.py $VERBOSE $ASYNC $UPDATE --regression-type $REGRESSION_TYPE --run-dir $RUN_DIR --commit-hash-before $previous_commit --commit-hash-after $current_commit"
    python ./infrastructure/src/python/terra_testing/main.py $VERBOSE $ASYNC $UPDATE --regression-type $REGRESSION_TYPE --run-dir $RUN_DIR --commit-hash-before $previous_commit --commit-hash-after $current_commit

    exit $?
elif [[ $COMMIT_BEFORE && $COMMIT_AFTER ]]; then
    # We can get all of the changed files between two commits
    git diff --name-only $COMMIT_BEFORE $COMMIT_AFTER > $RUN_DIR/changed_files.txt

    echo "python ./infrastructure/src/python/terra_testing/main.py $VERBOSE $ASYNC $UPDATE --regression-type $REGRESSION_TYPE --run-dir $RUN_DIR --commit-hash-before $COMMIT_BEFORE --commit-hash-after $COMMIT_AFTER"
    # We can then call the terra testing script with the regression type and the run directory
    python ./infrastructure/src/python/terra_testing/main.py $VERBOSE $ASYNC $UPDATE --regression-type $REGRESSION_TYPE --run-dir $RUN_DIR --commit-hash-before $COMMIT_BEFORE --commit-hash-after $COMMIT_AFTER

    # We need to exit with the exit code of the python script
    exit $?
fi


# ./scripts/terra_testing.sh -r Periodic -b main -n 5 -v