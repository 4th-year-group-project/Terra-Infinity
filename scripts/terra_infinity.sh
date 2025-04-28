#!/bin/bash
set -e  # Exit immediately if a command exits with non-zero status

current_dir=$(pwd)
sourceme_script="source ./sourceme"
python_script="python3 -m world_generation.master_script.master_script_server --host localhost --port 8000"

# Create logs directory if it doesn't exist
mkdir -p "$current_dir/logs"

echo "Starting server in the background..."
# Initialize conda properly and then deactivate
nohup bash -c "source ~/.bashrc && 
               $sourceme_script && 
               $python_script" > "$current_dir/logs/server.log" 2>&1 &

SERVER_PID=$!
echo "Server started with PID $SERVER_PID"

# Give the server a moment to start
sleep 5

echo "Starting renderer in the foreground..."
# Save the current directory
pushd ./renderer > /dev/null

ls .

# Fix the typo in the cmake command
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release || { echo "CMake configuration failed"; exit 1; }
cmake --build build -j16 || { echo "CMake build failed"; exit 1; }
./build/renderer || { echo "Renderer execution failed"; exit 1; }

# Return to original directory
popd > /dev/null

# Kill the server with the saved PID
echo "Killing server..."
kill $SERVER_PID || echo "Failed to kill server process"