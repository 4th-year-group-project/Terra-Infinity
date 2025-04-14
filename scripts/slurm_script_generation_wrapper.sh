#!/bin/bash

initialisation_commands=(
    "source /etc/profile.d/modules.sh" \
    "source /etc/profile.d/conda.sh"
)
execution_commands=(
    "conda activate /dcs/21/u2102661/Documents/Temp/Infrastructure-Testing/.env" \
    "python --version" \
    "python3 -c \"print('hello')\""
)

python infrastructure/src/python/slurm_script_generator.py \
    --batch-script-name="test_script" \
    --batch-script-path="./slurm/" \
    --job-name="test_job" \
    --partition="cpu-batch" \
    --time=1440 \
    --cpus-per-task=1 \
    --mem-per-cpu=1000 \
    --initialisation-commands "${initialisation_commands[@]}" \
    --execution-commands "${execution_commands[@]}" \
    --wait \
    -u

# TODO: Expand the bash script to include the option of automatically submitting the job to the cluster

# We will assume that we are already ssh into the cluster when running the python scripts if the argument $1 is provided
if [ "$1" == "-s" ]; then
    echo "Submitting job to the cluster"
    sbatch ./slurm/test_script.sbatch
    # Print the result of the previous command
    if [ $? -eq 0 ]; then
        echo "Job submitted successfully"
    else
        echo "Job submission failed"
    fi
fi


