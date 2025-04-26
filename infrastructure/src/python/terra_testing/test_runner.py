"""T
"""
import logging
import os
import subprocess
import time

from execution_context import ExecutionContext
from test_script_result import TestScriptResult, TestScriptResultAsync, TestScriptResultFailure, TestScriptResultSuccess
from testbench import Testbench

LOGGER = logging.getLogger(__name__)

class TestRunner:
    """TestRunner class is responsible for running the tests in parallel
    """
    execution_context: ExecutionContext
    tests_to_run: list[Testbench] = []
    running_tests: list[str] = []
    passed_tests: list[TestScriptResult] = []
    failed_tests: list[TestScriptResult] = []
    async_testbenches: list[TestScriptResult] = []


    def __init__(self, testbenches: list[Testbench], execution: ExecutionContext) -> None:
        self.tests_to_run = testbenches
        self.execution_context = execution


    def run_tests_serial(self, testbenches: list[Testbench]) -> tuple[list[TestScriptResult], list[TestScriptResult]]:
        """Run all the tests in parallel on this machine
        """
        for testbench in testbenches:
            self.running_tests.append(testbench.name)
            testbench.environment_variables["PROJECT_ROOT"] = self.execution_context.work_dir
            testbench.environment_variables["RUN_DIR"] = self.execution_context.run_dir
            LOGGER.info("Running testbench %s", testbench.name)
            # Combine the testbench environment variables with the current environment variables
            current_env = os.environ.copy()
            print("Current environment variables: ", current_env)
            combined_env = {**current_env, **testbench.environment_variables}
            if testbench.no_slurm:
                # Run the process in a blocking way
                proc = subprocess.Popen(
                    testbench.script_path,
                    env=combined_env,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                )
                LOGGER.info("Running command %s", f"{testbench.script_path}")
            else:
                proc = subprocess.Popen(
                    ["./scripts/slurm_job_launcher_wrapper.sh", testbench.script_path],
                    env=combined_env,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                )
                LOGGER.info("Running command %s", f"./scripts/slurm_job_launcher_wrapper.sh {testbench.script_path}")
            # Wait for the process to finish
            LOGGER.info("Waiting for testbench %s to finish", testbench.name)
            proc.wait()
            LOGGER.info("Testbench %s has finished", testbench.name)
            LOGGER.info("Return code: %s", proc.returncode)
            if proc.returncode != 0:
                LOGGER.error("Testbench %s has failed with return code %s", testbench.name, proc.returncode)
                # After the testbench is finished we want to output the stdout and stderr of the testbench
                f_stdout = "\n    ".join([line.decode("utf-8").rstrip() for line in proc.stdout.readlines()])
                f_stderr = "\n    ".join([line.decode("utf-8").rstrip() for line in proc.stderr.readlines()])
                LOGGER.info("Testbench %s:\n    %s", testbench.name, f_stdout)
                LOGGER.error("Testbench %s:\n    %s", testbench.name, f_stderr)
                self.failed_tests.append(TestScriptResultFailure(
                    name=testbench.name,
                    description=testbench.description,
                    message=f"Test failed with return code {proc.returncode}"
                ))
            else:
                LOGGER.info("Testbench %s has passed with return code %s", testbench.name, proc.returncode)
                # After the testbench is finished we want to output the stdout and stderr of the testbench
                f_stdout = "\n    ".join([line.decode("utf-8").rstrip() for line in proc.stdout.readlines()])
                f_stderr = "\n    ".join([line.decode("utf-8").rstrip() for line in proc.stderr.readlines()])
                LOGGER.info("Testbench %s:\n    %s", testbench.name, f_stdout)
                LOGGER.error("Testbench %s:\n    %s", testbench.name, f_stderr)
                self.passed_tests.append(TestScriptResultSuccess(
                    name=testbench.name,
                    description=testbench.description,
                    message=f"Test passed with return code {proc.returncode}"
                ))
            self.running_tests.remove(testbench.name)
            self.tests_to_run.remove(testbench)
            LOGGER.info(
                "Job are still running (%s left to run, %s passed, %s failed)",
                len(self.tests_to_run),
                len(self.passed_tests),
                len(self.failed_tests),
            )
        LOGGER.info("All tests have finished")
        return self.passed_tests, self.failed_tests, self.async_testbenches


    def run_tests(self, testbenches: list[Testbench]) -> tuple[list[TestScriptResult], list[TestScriptResult]]:
        """Run tests in parallel
        """
        proc_list: list[subprocess.Popen[bytes]] = []
        if self.execution_context.regression_args["async"]:
            LOGGER.info("Tests will be run asynchronously and will not block the main process")
            LOGGER.info("The results will need to be checked manually once all testbenchs have finished")
        for testbench in testbenches:
            self.running_tests.append(testbench.name)
            testbench.environment_variables["PROJECT_ROOT"] = self.execution_context.work_dir
            testbench.environment_variables["RUN_DIR"] = self.execution_context.run_dir
            LOGGER.info("Running testbench %s", testbench.name)
            # Combine the testbench environment variables with the current environment variables
            current_env = os.environ.copy()
            print("Current environment variables: ", current_env)
            combined_env = {**current_env, **testbench.environment_variables}
            if testbench.no_slurm:
                proc_list.append(subprocess.Popen(
                    testbench.script_path,
                    env=combined_env,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                ))
                LOGGER.info("Running command %s", f"{testbench.script_path}")
            else:
                proc_list.append(subprocess.Popen(
                    ["./scripts/slurm_job_launcher_wrapper.sh", testbench.script_path],
                    env=combined_env,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                ))
                LOGGER.info("Running command %s", f"./scripts/slurm_job_launcher_wrapper.sh {testbench.script_path}")
        LOGGER.info(
            "Job are still running (%s running, %s passed, %s failed, %s async launched)",
            len(self.running_tests),
            len(self.passed_tests),
            len(self.failed_tests),
            len(self.async_testbenches),
        )
        while self.tests_to_run:
            LOGGER.debug(
                "Checking if any testbenches have finished: %s testbenches %s procs",
                len(self.tests_to_run),
                len(proc_list)
            )
            for (proc, testbench) in zip(proc_list, testbenches, strict=False):
                LOGGER.debug("Checking testbench %s", testbench.name)
                poll = proc.poll()
                LOGGER.debug("Poll: %s", poll)
                if poll is not None:
                    LOGGER.debug("Testbench %s has finished", testbench.name)
                    LOGGER.debug("Return code: %s", proc.returncode)
                    # After the testbench is finished we want to output the stdout and stderr of the testbench
                    f_stdout = "\n    ".join([line.decode("utf-8").rstrip() for line in proc.stdout.readlines()])
                    f_stderr = "\n    ".join([line.decode("utf-8").rstrip() for line in proc.stderr.readlines()])
                    # if f_stdout:
                    #     LOGGER.info("Testbench %s:\n    %s", testbench.name, f_stdout)
                    # if f_stderr:
                    #     LOGGER.error("Testbench %s:\n    %s", testbench.name, f_stderr)
                    LOGGER.info("Testbench %s:\n    %s", testbench.name, f_stdout)
                    LOGGER.error("Testbench %s:\n    %s", testbench.name, f_stderr)
                    # If the testbenches are being run asynchronously and the testbench is being run on slurm
                    # then we need to record it as an async testbench
                    if self.execution_context.regression_args["async"] and not testbench.no_slurm and proc.returncode == 0:
                        LOGGER.info("Testbench %s has been run asynchronously", testbench.name)
                        self.async_testbenches.append(TestScriptResultAsync(
                            name=testbench.name,
                            description=testbench.description,
                            message="Testbench has been run asynchronously, check the logs for the results"
                        ))
                    # Any other testbench condition is ran normally until the testbench is finished and then we check
                    # the return code and report the result as appropriate
                    elif proc.returncode != 0:
                        LOGGER.error("Testbench %s has failed with return code %s", testbench.name, proc.returncode)
                        self.failed_tests.append(TestScriptResultFailure(
                            name=testbench.name,
                            description=testbench.description,
                            message=f"Test failed with return code {proc.returncode}"
                        ))
                    else:
                        LOGGER.info("Testbench %s has passed with return code %s", testbench.name, proc.returncode)
                        self.passed_tests.append(TestScriptResultSuccess(
                            name=testbench.name,
                            description=testbench.description,
                            message=f"Test passed with return code {proc.returncode}"
                        ))
                    self.running_tests.remove(testbench.name)
                    self.tests_to_run.remove(testbench)
                    proc_list.remove(proc)
                else:
                    LOGGER.debug("Testbench %s is still running", testbench.name)
            LOGGER.info(
                "Job are still running (%s running, %s passed, %s failed, %s async launched)",
                len(self.running_tests),
                len(self.passed_tests),
                len(self.failed_tests),
                len(self.async_testbenches),
            )
            LOGGER.debug("Testbenches still running: %s", [test for test in self.running_tests])
            LOGGER.debug("Number of testbenches still running: %s", len(self.tests_to_run))
            LOGGER.debug("Sleeping for 30 seconds before checking again")
            # # wait 0.5 minute before checking again
            time.sleep(1)
        return self.passed_tests, self.failed_tests, self.async_testbenches
