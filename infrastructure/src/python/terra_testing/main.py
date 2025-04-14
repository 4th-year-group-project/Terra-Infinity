# This is the main file for the infrastructure testing project

import json
import sys
import os
import subprocess
import time
import logging
import argparse
import fnmatch
import dataclasses

from testbench import Testbench
from banner import Banner
from execution_context import ExecutionContext
from test_runner import TestRunner

import yaml



@dataclasses.dataclass
class ModifiedFiles:
    files: list[str]

    def __init__(
        self,
        files: list[str],
    ) -> None:
        self.files = files

    def match_globs(self, globs: list[str]) -> list[str]:
        LOGGER.debug("%s", [file for file in self.files if any(fnmatch.fnmatch(file, glob) for glob in globs)])
        return [file for file in self.files if any(fnmatch.fnmatch(file, glob) for glob in globs)]



LOGGER = logging.getLogger(__name__)
PROJECT_DIR = os.environ.get("PROJECT_ROOT")


def _read_testbench_file(file_path: str, exec_context: ExecutionContext) -> list[Testbench]:
    testbenches = []
    # This is a yaml file
    with open(file_path, "r", encoding="utf-8") as file:
        testbench_data = yaml.safe_load(file)
        testbench_data = testbench_data["testbenches"]
        for testbench in testbench_data:
            testbenches.append(
                Testbench(
                    name=testbench["name"],
                    description=testbench["description"],
                    execution_context=exec_context,
                    no_slurm=testbench["no_slurm"],
                    slurm_args=testbench["slurm_args"] if "slurm_args" in testbench else {},
                    patterns=testbench["patterns"],
                    exclude_patterns=testbench["exclude_patterns"],
                    environment_variables=testbench["environment_variables"],
                    commands=testbench["commands"],
                )
            )

        for testbench in testbenches:
            LOGGER.info("Testbench found:%s", str(testbench))
    return testbenches


def _set_modified_files(file_path: str) -> ModifiedFiles | None:
    modified_files = []
    try:
        with open(file_path, "r", encoding="utf-8") as file:
            modified_files = [line.strip() for line in file if line.strip()]
        return ModifiedFiles(modified_files)
    except FileNotFoundError:
        LOGGER.warning("No file found at %s", file_path)
        return None


def _get_git_head_hash() -> str:
    # We want to get this information from running the command git log -1 --pretty=format:"%H %s %an %ad %d"
    # We are using subprocess to run the command
    git_log = subprocess.run(
        ["git", "log", "-1", "--pretty=format:%H \n%s \n%an \n%ad \n%d"],
        capture_output=True,
        check=True
    )
    git_log = git_log.stdout.decode("utf-8").split("\n")
    sha = git_log[0].strip()
    return sha




def _output_results(
    successful_tbs: list[Testbench],
    failed_tbs: list[Testbench],
    async_tbs: list[Testbench],
    exec_context: ExecutionContext
) -> None:
    logger_string = []
    logger_string.append("====================================")
    logger_string.append("")
    logger_string.append("All testbenches have been run")
    logger_string.append(f"In total {len(successful_tbs) + len(failed_tbs)} testbenches have been run")
    logger_string.append(f"{len(successful_tbs)} testbenches have passed")
    logger_string.append(f"{len(failed_tbs)} testbenches have failed")
    logger_string.append("")
    logger_string.append("Passed tests:")
    logger_string.extend([str(test) for test in successful_tbs] if successful_tbs else ["    No passed tests"])
    logger_string.append("")
    logger_string.append("Failed tests:")
    logger_string.extend([str(test) for test in failed_tbs] if failed_tbs else ["    No failed tests"])
    logger_string.append("")
    logger_string.append("Async tests:")
    logger_string.extend([str(test) for test in async_tbs] if async_tbs else ["    No async tests"])
    LOGGER.info("\n    ".join(logger_string))
    with open(os.path.join(exec_context.run_dir, "terra_testing_results.json"), "w", encoding="utf-8") as file:
        json.dump(
            {
                "successful_testbenches": [test.dumps() for test in successful_tbs],
                "failed_testbenches": [test.dumps() for test in failed_tbs],
                "async_testbenches": [test.dumps() for test in async_tbs],
            },
            file,
            indent=4
        )


def _run(work_dir: str, args: argparse.Namespace) -> int:
    test_pattern_path = os.path.join(work_dir, "infrastructure/src/yaml/test_patterns.yaml")
    run_dir = os.path.join(work_dir, f"run-Precommit-{time.strftime('%Y-%m-%d-%H-%M-%S', time.gmtime(time.time()))}")
    # Replace the run_dir default value with the run_dir from the args
    if args.run_dir:
        run_dir = args.run_dir
    if not os.path.exists(run_dir):
        os.makedirs(run_dir)
    commit_hash: str = args.commit_hash_after if args.commit_hash_after else _get_git_head_hash()
    exec_context: ExecutionContext = ExecutionContext(
        work_dir,
        run_dir,
        commit_hash,
        args.regression_type,
        args.__dict__
    )
    exec_context.write_to_json()
    testbenches: list[Testbench] = _read_testbench_file(test_pattern_path, exec_context)
    modified_files: ModifiedFiles = _set_modified_files(os.path.join(run_dir, "changed_files.txt"))
    list_of_testbenches_to_run: list[Testbench] = []
    for testbench in testbenches:
        # We are are excluding testbenches that do not match the patterns of the modified files
        if testbench.patterns and not modified_files.match_globs(testbench.patterns):
            continue
        elif testbench.exclude_patterns and modified_files.match_globs(testbench.exclude_patterns):
            continue
        list_of_testbenches_to_run.append(testbench)
    LOGGER.debug("List of testbenches to run: %s", [testbench.name for testbench in list_of_testbenches_to_run])
    for testbench in list_of_testbenches_to_run:
        LOGGER.info("Building testbench: %s", testbench.name)
        testbench.build_testbench()
    test_runner = TestRunner(list_of_testbenches_to_run, exec_context)
    # successful_testbenches, failed_testbenches, async_testbenches = test_runner.run_tests(list_of_testbenches_to_run)
    successful_testbenches, failed_testbenches, async_testbenches = test_runner.run_tests_serial(list_of_testbenches_to_run)
    _output_results(successful_testbenches, failed_testbenches, async_testbenches, exec_context)
    if failed_testbenches:
        return 1
    return 0

def _parse_args(args):
    parser = argparse.ArgumentParser(description="Infrastructure Testing")
    parser.add_argument(
        "--regression-type",
        help="The type of regression to run",
        type=str,
        default="Precommit",
    )
    parser.add_argument(
        "--run-dir",
        help="The directory to run the tests in",
        type=str,
        required=True,
    )
    parser.add_argument(
        "--commit-hash-before",
        help="The commit hash before the changes",
        type=str,
    )
    parser.add_argument(
        "--commit-hash-after",
        help="The commit hash after the changes",
        type=str,
    )
    parser.add_argument(
        "-v", "--verbose",
        help="Increase output verbosity",
        action="store_true",
    )
    parser.add_argument(
        "-a", "--async",
        help="Run the testbenches asynchronously",
        action="store_true",
    )
    parser.add_argument(
        "-u", "--update",
        help="Overwrite the testbench run scripts",
        action="store_true",
    )
    return parser.parse_args(args)

def _main(argv):
    args = _parse_args(argv)
    logging.basicConfig(
        filename=os.path.join(args.run_dir, "terra_testing.log"),
        filemode="w",
        format="%(asctime)s - %(levelname)s - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
        level=logging.DEBUG if args.verbose else logging.INFO,
    )
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
    console.setFormatter(formatter)
    logging.getLogger('').addHandler(console)
    banner = Banner()
    banner.print_banner()
    return _run(PROJECT_DIR, args)


if __name__ == "__main__":

    def main(argv):
        try:
            result = _main(argv)
            sys.exit(result)
        except KeyboardInterrupt:
            LOGGER.info("Exiting...")
            sys.exit(1)
        except Exception as _:
            LOGGER.exception("An uncaptured exception occurred")
            sys.exit(1)

    main(argv=sys.argv[1:])
