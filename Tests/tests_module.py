import subprocess
from subprocess import CalledProcessError
import sys
import os
import platform

from output import TestError, TestCompileError, report, print_nonl


def copy_sources_for_list():
    """ Copies the extra object generated source for the list test """
    try:
        os.rename(os.path.join("..", "src", "Data_Structures",
                               "list_deep_test_object.c"),
                  os.path.join("Data_Structures", "list_test_object.c"))
    except:
        print_nonl("During preparation of the list test, could not copy "
                   "the generated sources")


def cleanup_for_list():
    """ Cleansup the copied file for the list test """
    try:
        os.remove(os.path.join("Data_Structures", "list_test_object.c"))
    except:
        pass


class Test:
    """
    Defines a single test of the library
    """
    def __init__(self, name, sources, root,
                 action_before_compile=None,
                 action_after_test=None):
        self.name = name
        # list of sources to compile for the test
        self.sources = sources
        # root directory of the test's main source
        self.root = root
        # if not None this is a function to run before compiling
        self.action_before_compile = action_before_compile
        # if not None this is a function to run after test is ran
        self.action_after_test = action_after_test

    def before_compile(self):
        """
        Runs a given action before the test's compilation or does nothing
        """
        if self.action_before_compile is not None:
            self.action_before_compile()

    def after_test(self):
        """
        Performs a given action after a test has concluded or does nothing
        """
        if self.action_after_test is not None:
            self.action_after_test()

    def compile_test(self, dynamic, compiler, verbose, logFile):
        """Compiles a single test file from the tests depending on the compiler
           and library linking type

            --dynamic: A boolean value denoting if the linking should be done
            with a dynamic/shared library or not (static)
            --compiler: The name of the compiler with which to do the compiling
            --verbose: A boolean value denoting if the output to stdout
            should be verbose or not
            --logFile: The name of the logfile to print the output
        """
        if(dynamic):
            target = "test_shared"
        else:
            target = "test_static"

        sources_string = "__TEST_SOURCES="
        for s in self.sources:
            sources_string = sources_string+s+","
        sources_string = sources_string[:-1]

        if(platform.system() == 'Windows'):
            sconsCall = ["scons.py", target, '-Q', 'COMPILER=' + compiler,
                         sources_string]
        else:
            sconsCall = "{} scons.py {} -Q COMPILER={} {}".format(
                sys.executable, target, compiler, sources_string)

        try:
            p = subprocess.Popen(sconsCall, cwd="..", shell=True,
                                 stdout=subprocess.PIPE)
            for line in p.stdout:
                if(verbose):
                    print("\t\t"+line.decode("utf8"))
            #wait till the subprocess ends
            ret = p.poll()
            while ret is None:
                ret = p.poll()
            if ret != 0:
                report(logFile,
                       "\t\tDuring compiling test {0} the {1} compiler"
                       " returned unsucessfully with return code {2}".format(
                           self.name, compiler, ret))
                msg = ""
                if p.stdout is not None:
                    for line in p.stdout:
                        line = line.decode("utf8")
                        msg = msg + line
                        report(logFile, "\t\t"+line)
                raise TestCompileError(self.name, msg)

        except CalledProcessError as err:
            report(logFile, "\t\tThere was an error while invoking the "
                   "compiler for test \"{0}\"".format(self.name))
            report(logFile, "\t\tThe compiler returned [{0}] and the output "
                   "was:".format(err.returncode))
            raise TestCompileError(self.name, err.output)
        #success
        if(verbose):
            print("\t-> Succesfully compiled Test \"{0}\"".format(self.name))

    def run_test(self, logFile, testExec, verbose, fail_fast):
        """
            Tests a compiled test executable and compared it to the expected
            output
            Arguments:

            --logFile: The name of the logfile
            --testExec: The name of the test executable
            --verbose: A boolean flag denoting if the tests should be verbose
                      or not
            --fail_fast: A boolean flag denoting if the tests should fail fast
        """
        print_nonl("\t--> Testing \"{}\"...".format(self.name))
        logFile.write("\t--> Testing \"{}\"...".format(self.name))
        #try to open the expected outputfile
        try:
            f = open(os.path.join(self.root,
                                  "{}.expect".format(self.name)), 'r')
            expected_output = True
        except:
            expected_output = False
        #call the test
        p = subprocess.Popen(os.path.join('.', testExec),
                             cwd=self.root, stdout=subprocess.PIPE)

        expected_stdout_lineN = 1
        expected_stdout = None
        line = None
        for line in p.stdout:
            #read each line
            line = line.decode("utf8")
            line = line.replace("\r\n", "\n")
            #if it's just a position reporting line log it and continue
            if "passed >>" in line:
                logFile.write("\t\t\t{}\n".format(line))
                continue
            #if not then let's check the expected stdout line if we got one
            if expected_output:
                expected_stdout = f.readline()
            if not expected_output or line != expected_stdout:
                # if it's an EXPECT() macro error, report it
                if "*ERROR*:" in line:
                    report(logFile, "FAILED!\n\t {}".format(line))
                    if fail_fast:
                        raise TestError(self.name, expected_stdout_lineN)
                # else it's an expected std output error
                else:
                    report(logFile, "\n\t\tTest failed at expected "
                           "output line \"[{}]\"\n".format(
                               str(expected_stdout_lineN)))
                    report(logFile, "\n\t\tExpected:\n\t\t\t{}\n\t\t"
                           "Found:\n\t\t\t{}\n".format(
                               str(expected_stdout), str(line)))
                raise TestError(self.name, expected_stdout_lineN)
                expected_stdout_lineN += 1
        #wait till the test subprocess ends
        ret = p.poll()
        while ret is None:
            ret = p.poll()
        #success
        if ret is 0:
            report(logFile, "OK!\n")
        else:
            report(logFile, "FAILED!\n\t\tUnexpected Return Value. Test "
                   "executable probably segfaulted.")
            if line is not None:
                line = line.partition("[")[2]
                line = line.partition("]")[0]
                report(logFile,
                       "\t\tLast line we are sure succesfully executed "
                       "is [{}]\n".format(str(line)))
            raise TestError(self.name, line)

        return True

    def debug_test(self, logFile, testExec):
        """
            Debugs a compiled test executable by just spitting out its output

            --logFile: The name of the logfile
            --testExec: The name of the test executable
        """
        report(logFile,
               "\t**Testing \"{}\" in Debug Mode**\n".format(self.name))
        p = subprocess.Popen(os.path.join('.', testExec), cwd=self.root,
                             stdout=subprocess.PIPE)
        for line in p.stdout:  # for every line of the output
            line = line.decode("utf8")
            line = line.replace("\r\n", "\n")
            print_nonl("\t" + line)
            logFile.write(line)
        #wait till the subprocess ends
        ret = p.poll()
        while ret is None:
            ret = p.poll()


# The list of tests. If implementing more tests, add them here
tests_list = [
    Test("dynamicarray",
         [os.path.join("Data_Structures", "dynamicarray.c")],
         "Data_Structures"),

    Test("hashmap",
         [os.path.join("Data_Structures", "hashmap.c")],
         "Data_Structures"),

    Test("hashmap_shallow",
         [os.path.join("Data_Structures", "hashmap_shallowcopy.c")],
         "Data_Structures"),

    Test("hashmap_deep",
         [os.path.join("Data_Structures", "hashmap_deepcopy.c")],
         "Data_Structures"),

    Test("intrusive_list",
         [os.path.join("Data_Structures", "intrusive_list.c")],
         "Data_Structures"),

    Test("list",
         [
             os.path.join("Data_Structures", "list.c"),
             os.path.join("Data_Structures", "list_test_object.c"),
             os.path.join("ExtraObjects", "test.c")
         ],
         "Data_Structures",
         copy_sources_for_list,
         cleanup_for_list),

    Test("string_accessor",
         [os.path.join("String", "string_accessor.c")],
         "String"),

    Test("string_additions",
         [os.path.join("String", "string_additions.c")],
         "String"),

    Test("string_conversion",
         [os.path.join("String", "string_conversion.c")],
         "String"),

    Test("string_copying",
         [os.path.join("String", "string_copying.c")],
         "String"),

    Test("string_filedescriptors",
         [os.path.join("String", "string_filedescriptors.c")],
         "String"),

    Test("string_init",
         [os.path.join("String", "string_init.c")],
         "String"),

    Test("string_iteration",
         [os.path.join("String", "string_iteration.c")],
         "String"),

    Test("string_positionalretrieval",
         [os.path.join("String", "string_positionalretrieval.c")],
         "String"),

    Test("string_removals",
         [os.path.join("String", "string_removals.c")],
         "String"),

    Test("string_replace",
         [os.path.join("String", "string_replace.c")],
         "String"),

    Test("string_retrieval",
         [os.path.join("String", "string_retrieval.c")],
         "String"),

    Test("string_traversal",
         [os.path.join("String", "string_traversal.c")],
         "String"),

    Test("system_files",
         [os.path.join("System", "system_files.c")],
         "System"),

    Test("textfile_lineend",
         [os.path.join("TextFile", "textfile_lineend.c")],
         "TextFile"),

    Test("textfile_read",
         [os.path.join("TextFile", "textfile_read.c")],
         "TextFile"),

    Test("textfile_write",
         [os.path.join("TextFile", "textfile_write.c")],
         "TextFile"),

    Test("worker_pool",
         [os.path.join("Parallel", "test_worker_pool.c")],
         "Parallel")

    Test("test_log_simple",
         [os.path.join("Log", "test_log_simple.c")],
         "Log")
]
