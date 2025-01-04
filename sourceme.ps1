# Description: This script is used to create the conda environment for the project and set up all 
# of the necessary environment variables. 

$ErrorActionPreference = "Stop"

echo "Setting up the conda environment for the project"

$Env:PROJECT_ROOT = $PWD
$Env:INFRA_ROOT = "$Env:PROJECT_ROOT\infrastucture"
$Env:PRISM_ROOT = "$Env:PROJECT_ROOT\renderer\src\prism"

# Check if the directory "$PROJECT_ROOT\.env" exists and if not create it
if (-not (Test-Path "$Env:PROJECT_ROOT\.env")) {
    New-Item -ItemType Directory -Path "$Env:PROJECT_ROOT\.env"
}

# We need to run conda initilsation to ensure that conda commands will work
conda init powershell | Out-Null

# There is one possible parameter for this script which is "--update" which will update the conda 
# environment instead of just directly activating it if it already exists
if ($args[0] -eq "--update") {
    . "$Env:PROJECT_ROOT\environment\build_environment.ps1" -Action -su -Path "$Env:PROJECT_ROOT\.env"
}  else {
    # We now need to source the build_environment.ps1 file to create the conda environment
    . "$Env:PROJECT_ROOT\environment\build_environment.ps1" -Action -s -Path "$Env:PROJECT_ROOT\.env"
}

# Cleans and removes the conda environment
# . "$Env:PROJECT_ROOT\environment\build_environment.ps1" -Action -c -Path "$Env:PROJECT_ROOT\.env"

echo "Finished setting up the conda environment for the project"
# Set errors to be non-terminating
$ErrorActionPreference = "Continue"