import dataclasses
import json
import os
import time
from typing import Any

import git


@dataclasses.dataclass
class ExecutionContext:
    work_dir: str
    run_dir: str
    run_date: str | None = None
    regression_id: str | None = None
    regression_type: str | None = None
    regression_args: dict[str, Any] | None = None
    author: str | None = None
    author_email: str | None = None
    commit_hash: str | None = None
    branch: str | None = None
    message: str | None = None
    authored_date: str | None = None
    committed_date: str | None = None

    def __init__(
        self,
        work_dir: str,
        run_dir: str,
        commit_hash: str,
        regression_type: str,
        regression_args: dict[str, Any],
        run_date: str | None = None,
        regression_id: str | None = None,
        author: str | None = None,
        author_email: str | None = None,
        branch: str | None = None,
        message: str | None = None,
        authored_date: str | None = None,
        committed_date: str | None = None,
    ) -> None:
        self.work_dir = work_dir
        self.run_dir = run_dir
        self.commit_hash = commit_hash
        self.regression_type = regression_type
        self.regression_args = regression_args

        if run_date and regression_id and author and author_email and branch and message and authored_date and committed_date:
            self.run_date = run_date
            self.regression_id = regression_id
            self.author = author
            self.author_email = author_email
            self.branch = branch
            self.message = message
            self.authored_date = authored_date
            self.committed_date = committed_date
        else:
            start_time = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(time.time()))
            self.run_date = start_time
            self.regression_id = hash(start_time)
            repo = git.Repo(work_dir)
            assert not repo.bare
            for branch in repo.remotes.origin.refs:
                if str(branch.commit) == commit_hash:
                    self.branch = branch.name.removeprefix("origin/")
            commit: git.Commit = repo.commit(commit_hash)
            self.author = str(commit.author)
            self.author_email = commit.author.email
            self.message = commit.message
            self.commit_hash = commit.hexsha
            self.authored_date = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(commit.authored_date))
            self.committed_date = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(commit.committed_date))

    def __str__(self) -> str:
        return f"""
        Work Directory: {self.work_dir}
        Run Directory: {self.run_dir}
        Run Date: {self.run_date}
        Regression ID: {self.regression_id}
        Regression Type: {self.regression_type}
        Regression Args: {self.regression_args}
        Author: {self.author}
        Author Email: {self.author_email}
        Commit Hash: {self.commit_hash}
        Branch: {self.branch}
        Message: {self.message}
        Authored Date: {self.authored_date}
        Committed Date: {self.committed_date}
        """

    def to_dict(self) -> dict[str, Any]:
        return {
            "work_dir": self.work_dir,
            "run_dir": self.run_dir,
            "run_date": self.run_date,
            "regression_id": self.regression_id,
            "regression_type": self.regression_type,
            "regression_args": self.regression_args,
            "author": self.author,
            "author_email": self.author_email,
            "commit_hash": self.commit_hash,
            "branch": self.branch,
            "message": self.message,
            "authored_date": self.authored_date,
            "committed_date": self.committed_date,
        }

    def write_to_json(self) -> None:
        with open(os.path.join(self.run_dir, "regression_run_data.json"), "w", encoding="utf-8") as file:
            json.dump(self.to_dict(), file, indent=4)
