import json
import os
import subprocess
import time

import requests

# Run test using the following command
# python3 -m world_generation.integration_tests.end_to_end

def test_data_path():
    folder_path = "world_generation/integration_tests/test_data/"
    return folder_path


def generate_data_path():
    folder_path = "world_generation/master_script/dump/"
    return folder_path


def load_test_bin_files(test_data_path):
    # Load all .bin files in the test_data_path directory
    bin_files = []
    for file in os.listdir(test_data_path):
        if file.endswith(".bin"):
            bin_files.append(os.path.join(test_data_path, file))
    return bin_files


def load_bin_files():
    # Load all .bin files in the dump directory
    data_path = generate_data_path()
    bin_files = []
    for file in os.listdir(data_path):
        if file.endswith(".bin"):
            bin_files.append(os.path.join(data_path, file))
    return bin_files


def correct_parameters():
    # Load parameters from the JSON file
    with open("world_generation/integration_tests/test_data/parameters_correct_chunk.json") as f:
        parameters = json.load(f)
    return parameters


def missing_parameters():
    # Load parameters from the JSON file
    with open("world_generation/integration_tests/test_data/parameters_missing_param.json") as f:
        parameters = json.load(f)
    return parameters


def incorrect_format():
    # Load parameters from the file
    with open("world_generation/integration_tests/test_data/incorrect_format.txt") as f:
        parameters = f.read()
    return parameters


def start_server():
    # Start the server
    cmd = [
        "python3",
        "-m",
        "world_generation.master_script.master_script_server",
        "--host",
        "localhost",
        "--port",
        "8000",
    ]
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return process


def stop_server(process):
    process.terminate()  # Politely ask it to stop
    try:
        process.wait(timeout=5)  # Give it 5 seconds to exit cleanly
    except subprocess.TimeoutExpired:
        process.kill()  # Force kill if it hangs


def generate_correct_superchunk():
    loaded_parameters = correct_parameters()

    # Set up headers for JSON content type
    headers = {"Content-Type": "application/json"}

    # Parse base JSON text
    params = json.dumps(loaded_parameters)

    # Make the POST request with the raw JSON text
    response = requests.post("http://localhost:8000/superchunk", data=params, headers=headers)

    return response


def generate_incorrect_post():
    loaded_parameters = correct_parameters()

    # Set up headers for JSON content type
    headers = {"Content-Type": "application/json"}

    # Parse base JSON text
    params = json.dumps(loaded_parameters)

    # Make the POST request with the raw JSON text
    response = requests.post("http://localhost:8000/incorrect_path", data=params, headers=headers)

    return response


def generate_missing_params():
    loaded_parameters = missing_parameters()

    # Set up headers for JSON content type
    headers = {"Content-Type": "application/json"}

    # Parse base JSON text
    params = json.dumps(loaded_parameters)

    # Make the POST request with the raw JSON text
    response = requests.post("http://localhost:8000/superchunk", data=params, headers=headers)

    return response


def generate_incorrect_json():
    loaded_parameters = incorrect_format()

    # Set up headers for JSON content type
    headers = {"Content-Type": "application/json"}

    # Parse base JSON text
    params = loaded_parameters

    # Make the POST request with the raw JSON text
    response = requests.post("http://localhost:8000/superchunk", data=params, headers=headers)

    return response


def test_correct_superchunk():
    returned_data = generate_correct_superchunk()

    # Check if the response is successful
    assert returned_data.status_code == 200, f"Error with request: {returned_data.status_code}"

    # Check generated bin files match expected files

    test_data = load_test_bin_files(test_data_path())
    generated_data = load_bin_files()

    # Compare the number of files
    assert len(test_data) == len(generated_data), "Number of generated files does not match the expected number."
    # Compare the contents of each file

    for test_file in test_data:
        # Get the corresponding generated file
        generated_file = os.path.join(generate_data_path(), os.path.basename(test_file))

        # Check if the generated file exists
        assert os.path.exists(generated_file), f"Generated file {generated_file} does not exist."

        # Read the contents of both files
        with open(test_file, "rb") as f:
            test_data = f.read()
        with open(generated_file, "rb") as f:
            generated_data = f.read()

        # Compare the contents of the files
        assert test_data == generated_data, f"Contents of {test_file} and {generated_file} do not match."


def test_missing_params():
    returned_data = generate_missing_params()

    # Check if the response is successful
    assert returned_data.status_code == 400, f"Error with request: {returned_data.status_code}"

    # Check the error message
    error_message = returned_data.json().get("error")
    assert "missing required parameters" in error_message.lower(), "Error message does not indicate missing parameters."


def test_incorrect_request_path():
    # Make a request to an incorrect path
    response = generate_incorrect_post()

    # Check if the response is successful
    assert response.status_code == 404, f"Error with request: {response.status_code}"


def test_incorrect_json():
    # Make a request with incorrect JSON format
    response = generate_incorrect_json()

    # Check if the response is successful
    assert response.status_code == 400, f"Error with request: {response.status_code}"

    # Check the error message
    error_message = response.json().get("error")
    assert "invalid json format" in error_message.lower(), "Error message does not indicate invalid JSON format."


def clear_dump():
    # Clear the dump directory
    data_path = generate_data_path()
    for file in os.listdir(data_path):
        if file.endswith(".bin"):
            os.remove(os.path.join(data_path, file))


if __name__ == "__main__":
    # Start the server
    process = start_server()
    # Wait for the server to start
    time.sleep(5)  # Adjust this time as needed

    # Check server is running
    try:
        response = requests.get("http://localhost:8000/health")
        assert response.status_code == 200, "Server is not running."
    except requests.ConnectionError:
        raise RuntimeError("Server failed to start.")

    # Clear the dump directory
    clear_dump()

    # Run the test
    try:
        test_correct_superchunk()
        clear_dump()
        test_missing_params()
        clear_dump()
        test_incorrect_request_path()
        clear_dump()
        test_incorrect_json()
        print("All tests passed.")
    except AssertionError as e:
        print(f"Test failed: {e}")
    finally:
        # Clear the dump directory
        clear_dump()
        # Close the server
        stop_server(process)
