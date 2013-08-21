import sys


class TestCompileError(Exception):
    """
       A Test compile exception
    """
    def __init__(self, test_name, msg):
        self.test_name = test_name
        self.msg = msg
    
    def __str__(self):
        if self.msg != "":
            return ("\t**TEST-COMPILE-FAIL**: {} failed to compile. Error message: "
                    "{}".format(self.test_name, self.msg))
        else:
            return ("\t**TEST-COMPILE-FAIL**: {} failed to compile.".format(
                self.test_name)
            )

class TestError(Exception):
    """
       A simple Test Error exception, reporting test and line number
       line_num can be None if we don't actually know the number of
       the line of the error
    """
    def __init__(self, test_name, line_num=None):
        self.test_name = test_name
        self.line_num = line_num
    
    def __str__(self):
        if self.line_num is not None:
            return "\t**TEST-FAIL**: {} failed at line {}".format(
                self.test_name, self.line_num)
        else:
            return "\t**TEST-FAIL**: {} failed ".format(
                self.test_name)



class TestsFail(Exception):
    """
       An exception thrown when the tests for this build of the library
       must prematurely stop
    """
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return "**TESTS-FAILED**: {}".format(self.msg)

def print_nonl(msg):
    sys.stdout.write(msg)
    sys.stdout.flush()

def report(log_file, string):
    print(string)
    log_file.write(string)
