#!/bin/bash

# This is a script that has only be tested on the department machines but it might work for
# WSL. It will create a separate terminal window for the server to run on and then runs the
# renderer in the current terminal window. When the program ends it will kill the server to
# ensure a clean exit.
#
#

current_dir=$(pwd)

source "$current_dir/sourceme"

# Wait for the sourceme script to finish
if [[ $? -ne 0 ]]; then
    echo "Error: sourceme script failed to execute."
    exit 1
fi

echo "Current directory: $current_dir"

# We need to launch the python script on another process and then run the renderer on the current process
# Fix the python_script variable assignment (remove spaces)
python_script="python3 -m master_script.master_script_server --host localhost --port 8000"
sourceme_script="source ./sourceme"


# Launch the python script in a new terminal window if available, otherwise in background
echo "Starting server in the background..."
# Linux with various terminal emulators
if command -v gnome-terminal &> /dev/null; then
    gnome-terminal -- bash -c "source ~/.bashrc && source ~/.bash_profile  && conda deactivate && cd '$current_dir' && $sourceme_script && $python_script; exec bash"&
elif command -v xterm &> /dev/null; then
    xterm -e "source ~/.bashrc && source ~/.bash_profile  && conda deactivate && cd '$current_dir' && $sourceme_script && $python_script; exec bash"&
elif command -v konsole &> /dev/null; then
    konsole -e "source ~/.bashrc && source ~/.bash_profile  && conda deactivate && cd '$current_dir' && $sourceme_script && $python_script; exec bash"&
else
    echo "No suitable terminal emulator found."
fi
# Give the server a moment to start
sleep 5

echo "Starting renderer in the foreground..."
# Renderer command goes here

cd $current_dir/renderer


if [[ -z "$1" ]]; then
    echo "No argument provided. Using default target: 'department'."
    target="department"
else
    target="$1"
fi

# Set the target variable based on if the argument is "linux" or "department"
if [[ "$target" == "linux" ]]; then
    target="linux"
elif [[ "$target" == "department" ]]; then
    target="department"
else
    echo "Invalid argument. Using default target: 'department'."
    target="department"
fi

echo "Cleaning up previous builds..."

if [[ "$target" == "linux" ]]; then
    make l_clean
else
    make d_clean
fi

echo "Building for target: $target"

make $1 -j8

./build/renderer

# When the script ends we need to kill the server
echo "Killing server..."
kill $(ps aux | grep '[m]aster_script_server' | awk '{print $2}')