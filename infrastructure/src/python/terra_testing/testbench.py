import os
import dataclasses
from typing import Any
import logging
import subprocess

from slurm_script_generator import GenerateSlurmScript
from execution_context import ExecutionContext

LOGGER = logging.getLogger(__name__)

@dataclasses.dataclass
class Testbench:
    name: str
    description: str
    execution_context: ExecutionContext
    no_slurm: bool
    slurm_args: dict[str, Any]
    patterns: list[str]
    exclude_patterns: list[str]
    environment_variables: dict[str, str]
    commands: list[str]
    script_path: str = ""

    def __init__(
        self,
        name: str,
        description: str,
        execution_context: ExecutionContext,
        no_slurm: bool,
        slurm_args: dict[str, Any],
        patterns: list[str],
        exclude_patterns: list[str],
        environment_variables: list[dict[str, Any]],
        commands: list[str],
    ) -> None:
        self.name = name.replace(" ", "_")
        self.description = description
        self.execution_context = execution_context
        self.no_slurm = no_slurm
        self.slurm_args = slurm_args
        self.patterns = patterns
        self.exclude_patterns = exclude_patterns
        self.environment_variables = {}
        # This implementation has the ability to overwrite environment variables with the same key
        if not environment_variables:
            logging.debug("environment_variables is empty")
            self.environment_variables = {}
        else:
            for entry in environment_variables:
                if not entry:
                    logging.debug("environment_variables is empty")
                    continue
                if not isinstance(entry, dict):
                    raise TypeError("environment_variables must be a list of dictionaries")
                # We expect this to only be two key value pairs one for key and one for value
                if len(entry) != 2:
                    raise ValueError("environment_variables must be a list of dicts with only two key value pairs")
                self.environment_variables[entry["name"]] = entry["value"]
        self.commands = commands

    def __str__(self) -> str:
        return f"""
        Testbench: {self.name}
        Description: {self.description}"""

    # TODO: Implement this validation function to create a yaml specification for the testbench
    def validate_testbench(self) -> bool:
        pass

    def _build_bash_script(self, default_initialisation: list[str]) -> str:
        """
        This is a function that builds a bash script to run the commands of the testbench
        """
        # default_initialisation.insert(1, "source ~/.bashrc")
        script_path = os.path.join(self.execution_context.work_dir, "scripts", "generated_scripts", f"{self.name}.sh")
        bash_script: list[str] = []
        bash_script.append("#!/bin/bash")
        bash_script.append("")
        bash_script.append("# Initialisation commands")
        bash_script.extend(default_initialisation)
        bash_script.append("echo 'Running testbench as a bash script'")
        bash_script.append("")
        bash_script.append("# Testbench commands")
        bash_script.extend(self.commands)
        if self.execution_context.regression_args["update"]:
            with open(script_path, "w", encoding="utf-8") as file:
                file.write("\n".join(bash_script))
            LOGGER.info("Bash script created at %s", script_path)
            return script_path
        if os.path.exists(script_path):
            LOGGER.warning("The file already exists. Use the -u or --update flag to overwrite the file.")
            return script_path
        with open(script_path, "w", encoding="utf-8") as file:
            file.write("\n".join(bash_script))
        LOGGER.info("Bash script created at %s", script_path)
        return script_path

    def _set_default_slurm_args(self) -> None:
        """
        This is a function that sets the default slurm arguments for the slurm script
        """
        self.slurm_args = {
            "partition": "cpu-batch",
            "cpus_per_task": 1,
            "mem_per_cpu": 1000,
            "time": 720,
            "output": f"joboutput_{self.name}_%j.out",
            "error": f"joberror_{self.name}_%j.err",
            "wait": True,
            "export": "ALL," + ",".join([f"{key}='{value}'" for key, value in self.environment_variables.items()])
        }

    def _build_slurm_script(self, default_initialisation: list[str]) -> str:
        script_path = os.path.join(self.execution_context.work_dir, "slurm", f"{self.name}.sbatch")
        if not self.slurm_args:
            self._set_default_slurm_args()
        self._format_slurm_args()
        slurm_script = GenerateSlurmScript(
            batch_script_name=self.name,
            batch_script_path=os.path.split(script_path)[0],
            initialisation_commands=default_initialisation,
            execution_commands=self.commands,
            update=self.execution_context.regression_args["update"],
            **(self.slurm_args)
        )
        slurm_script.validate_args()
        formatted_slurm_script = slurm_script.format_slurm_args()
        slurm_script.generate_slurm_script(formatted_slurm_script)
        return script_path


    def _format_slurm_args(self) -> None:
        """
        This is a function that formats the slurm arguments of the testbench for the slurm script.
        """

        # Append the working directory to the output and error filenames provided in the slurm_args
        if self.slurm_args["output"]:
            self.slurm_args.update({"output": os.path.join(self.execution_context.run_dir, self.slurm_args["output"])})
        if self.slurm_args["error"]:
            self.slurm_args.update({"error": os.path.join(self.execution_context.run_dir, self.slurm_args["error"])})
        # Make sure that the environment variables are exported correctly to the slurm script
        if "export" not in self.slurm_args and self.environment_variables:
            self.slurm_args.update({"export": "ALL," + ",".join([f"{key}='{value}'" for key, value in self.environment_variables.items()])})
        elif "export" not in self.slurm_args:
            self.slurm_args.update({"export": "ALL"})
        if self.execution_context.regression_args["async"]:
            self.slurm_args.update({"wait": False})


    def build_testbench(self) -> str:
        """This function builds the testbench.

        This is a function that runs the commands of the testbench by creating a slurm batch script
        with slurm_script_generator.py and then submitting the batch script to the slurm queue and getting the results.
        """
        default_initialisation: list[str] = [
            "set -e",
            "source /etc/profile.d/modules.sh",
            "module purge",
            "source /etc/profile.d/conda.sh",
            f"source {self.execution_context.work_dir}/sourceme --testing",
        ]
        script_path = ""
        if self.no_slurm:
            # We are going to want to build a bash script that we execute instead
            script_path = self._build_bash_script(default_initialisation)
        else:
            script_path = self._build_slurm_script(default_initialisation)
        # We need to make the slurm scripts executable
        subprocess.run(
            ["chmod", "700", script_path],
            check=True
        )
        self.script_path = script_path
        return script_path
