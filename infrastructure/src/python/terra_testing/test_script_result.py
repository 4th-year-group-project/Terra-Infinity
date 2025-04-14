"""
This module contains classes for test script results
"""
from typing import Any

class TestScriptResult:
    """
    Base class for test script results
    """
    name: str
    description: str
    passed: bool | None
    message: str


    def __init__(
        self,
        name: str,
        description: str,
        passed: bool,
        message: str,
    ) -> None:
        self.name = name
        self.description = description
        self.passed = passed
        self.message = message

    def _flags(self, flag: bool | None) -> str:
        return "Unknown" if flag is None else "Passed" if flag else "Failed"


    def __str__(self) -> str:
        return f"""
        Test: {self.name}
        Description: {self.description}
        Status: {self._flags(self.passed)}
        Message: {self.message}"""


    def dumps(self) -> dict[str, Any]:
        """
        Serialize the object to a dictionary
        """
        return {
            "Testbench_name": self.name,
            "Description": self.description,
            "Status": self._flags(self.passed),
            "Message": self.message,
        }


class TestScriptResultSuccess(TestScriptResult):
    """
    Class for successful test script results
    """
    def __init__(
        self,
        name: str,
        description: str,
        message: str,
        passed: bool = True,
    ) -> None:
        super().__init__(name, description, passed, message)


class TestScriptResultFailure(TestScriptResult):
    """
    Class for failed test script results
    """
    def __init__(
        self,
        name: str,
        description: str,
        message: str,
        passed: bool = False,
    ) -> None:
        super().__init__(name, description, passed, message)

class TestScriptResultAsync(TestScriptResult):
    """
    Class for async test script results
    """
    def __init__(
        self,
        name: str,
        description: str,
        message: str,
        passed: bool | None = None,
    ) -> None:
        super().__init__(name, description, passed, message)
