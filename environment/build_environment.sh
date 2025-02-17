#!/bin/bash

####################################################################################################
# Help
####################################################################################################
Help()
{
    # Display Help
    echo This script is used to create a conda environment for the infrastructure testing
    echo
    echo The order of the arguments is important with the first argument being the action to be taken
    echo and the second argument being the path to the conda environment though the path is optional
    echo "Syntax: build_environment.sh [-h|c|s|su] [-p] <path>"
    echo "options:"
    echo "-h  | --help             Print this Help."
    echo "-c  | --clean            Clean the conda environment."
    echo "-s  | --setup            Setup the conda environment."
    echo "-su | --setup-update     Setup the conda environment and update it."
    echo
    echo "-p  | --path      The path to the conda environment."
    echo
}

####################################################################################################
# Functions
####################################################################################################

# Checks if a conda environment already exists
find_in_conda_env(){
    conda env list | grep "${@}" >/dev/null 2>/dev/null
}

# For all of these functions, the first argument is the conda environment path
create_conda_env(){
    # check if the conda environment path is passed
    if [[ -z "$1" ]]; then
        conda env create --name world-generation -f $PROJECT_ROOT/environment/environment.yaml
    else
        conda env create --prefix $1 -f $PROJECT_ROOT/environment/environment.yaml
    fi
}

activate_conda_env(){
    if [[ -z "$1" ]]; then
        conda activate world-generation
    else
        conda activate $1
    fi
    # We now need to ensure that the non-condo dependencies are installed
    pip install perlin-noise
}

update_conda_env(){
    if [[ -z "$1" ]]; then
        conda env update --name world-generation -f $PROJECT_ROOT/environment/environment.yaml
    else
        conda env update --prefix $1 -f $PROJECT_ROOT/environment/environment.yaml
    fi
}

deactivate_conda_env(){
    conda deactivate
}

remove_conda_env(){
    if [[ -z "$1" ]]; then
        conda env remove -n world-generation -y
    else
        conda env remove -p $1 -y
    fi
}

####################################################################################################
# Main
####################################################################################################
# set -e

# Ensure that a conda environment path has been passed in as the second argument and it has to equal "-p"
if [[ "$2" != "-p" && "$2" != "--path" && ! -z "$2" ]]; then
    echo "The conda environment path can only be passed in as the second argument "
    echo "$2 was passed instead"
    return 1
fi

case $1 in
    -h | --help)
        Help;;
    -c | --clean)
        echo "Removing the conda environment"
        deactivate_conda_env
        if [[ -z "$3" ]]; then
            echo "No conda environment path has been passed in will use the default path"
            remove_conda_env
        else
            remove_conda_env $3
        fi;;
    -su | --setup-update)
        if [[ -z "$3" ]]; then
            echo "No conda environment path has been passed in will use the default path"
            if find_in_conda_env world-generation; then
                echo "The conda environment exists already - updating the environment"
                update_conda_env
            else
                echo "Creating the conda environment"
                create_conda_env
            fi
            echo "Activating the conda environment"
            activate_conda_env
        else
            echo "The conda environment path is $3"
            if find_in_conda_env $3; then
                echo "The conda environment exists already - updating the environment"
                update_conda_env $3
            else
                echo "Creating the conda environment"
                create_conda_env $3
            fi
            echo "Activating the conda environment"
            activate_conda_env $3
        fi
        ;;
    -s | --setup)
        if [[ -z "$3" ]]; then
            echo "No conda environment path has been passed in will use the default path"
            if find_in_conda_env world-generation; then
                echo "The conda environment exists already"
            else
                echo "Creating the conda environment"
                create_conda_env
            fi
            echo "Activating the conda environment"
            activate_conda_env
        else
            echo "The conda environment path is $3"
            if find_in_conda_env $3; then
                echo "The conda environment exists already"
            else
                echo "Creating the conda environment"
                create_conda_env $3
            fi
            echo "Activating the conda environment"
            activate_conda_env $3
        fi;;
    *)
        echo "Invalid option: $1" 1>&2;;
esac