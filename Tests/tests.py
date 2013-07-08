import subprocess
from subprocess import CalledProcessError
import os
import shutil
import time
import platform
import sys

from testscompile import compileTest
from output import print_nonl

#the time by which to delay actions after compiling so as not to have conflicts
#after copying/deleting binaries
delay = 0.20


def configureLinking(dynamic, root, outName):
    """Deals with the library linking for each test"""
    if(dynamic):
        #if we are testing the dynamic lib in windows copy it to the
        #test  directory source so that the lib can be accessed
        if(platform.system() == 'Windows'):
            try:
                os.remove(os.path.join(root,outName))
            except:
                pass
            shutil.copy(os.path.join(".",outName),os.path.join(root,outName))


def report(log_file, string):
    print(string)
    log_file.write(string)

def  check_library_log(log_file, root):
    """
       Copies the library log file into the output and into the 
       tests log if and only if there is anything to report

       log_file - The file to which the tests log is saved
       root - The root directory where the current test is ran
    """
    refu_error_logf = os.path.join(root, "refuErrorLog")
    if(os.path.getsize(refu_error_logf) > 0):
        report(log_file, "--Start of Library Log--")
        l = open(refu_error_logf)
        for line in l:
            report(log_file, line)
        report(log_file, "--End of Library Log--")

class TestError(Exception):
    """
       A simple Test Error exception, reporting test and line number
       line_num can be None if we don't actually know the number of
       the line of the error
    """
    def __init__(self, test_name, line_num):
        self.test_name = test_name
        self.line_num = line_num
    
    def __str__(self):
        return "\t**TEST-FAIL**: {} failed at line {}".format(
            self.test_name, self.line_num)

class TestsFail(Exception):
    """
       An exception thrown when the tests for this build of the library
       must prematurely stop
    """
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return "**TESTS-FAILED**: {}".format(self.msg)

def test(root, fileName, logFile, testExec, verbose, fail_fast):
    """
        Tests a compiled test executable and compared it to the expected
        output
        Arguments:

        --root: The root directory of the test compared to the Tests
                directory
        --fileName: The filename of the test
        --logFile: The name of the logfile
        --testExec: The name of the test executable
        --verbose: A boolean flag denoting if the tests should be verbose
                  or not
        --fail_fast: A boolean flag denoting if the tests should fail fast
    """
    #Remove Tests/ part of the directory from the filename
    tempName = os.path.split(fileName)
    fileName = os.path.join(os.path.split(tempName[0])[1] , tempName[1])
    print_nonl("\t--> Testing \"{}\"...".format(fileName))
    logFile.write("\t--> Testing \"{}\"...".format(fileName))
    #open the expected outputfile
    f = open(fileName+'.expect', 'r')
    #call the test
    p = subprocess.Popen(os.path.join('.',testExec),
                         cwd=root,stdout=subprocess.PIPE)
    expected_stdout_lineN = 1
    line = None
    for line in p.stdout:
        #read each line
        line = line.decode("utf8");
        line = line.replace("\r\n","\n")
        #if it's just a position reporting line log it and continue
        if "passed >>" in line:
            logFile.write("\t\t\t{}\n".format(line))
            continue
        #if not then let's check the expected stdout line
        expected_stdout = f.readline()
        if line != expected_stdout:
            # if it's an EXPECT() macro error, report it
            if "*ERROR*:" in line:
                report(logFile, "FAILED!\n\t {}".format(line))
                if fail_fast:
                    raise TestError(fileName, expected_stdout_lineN)
            # else it's an expected std output error
            else:
                report(logFile, "\n\t\tTest failed at expected "
                            "output line \"[{}]\"\n".format(
                                str(expected_stdout_lineN)))
                report(logFile, "\n\t\tExpected:\n\t\t\t{}\n\t\t"
                            "Found:\n\t\t\t{}\n".format(
                                str(expected_stdout), str(line)))
                raise TestError(fileName, expected_stdout_lineN)
            expected_stdout_lineN += 1
    #wait till the test subprocess ends
    ret = p.poll()
    while(ret is None):
        ret=p.poll()
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
        raise TestError(fileName, line)

    return True

def debugTest(root, fileName, logFile, testExec):
    """
        Debugs a compiled test executable by just spitting out its output
        --root: The root directory of the test compared to the Tests directory
        --fileName: The filename of the test
        --logFile: The name of the logfile
        --testExec: The name of the test executable
    """
    #Remove Tests/ part of the directory from the filename
    tempName = os.path.split(fileName)
    fileName = os.path.join(os.path.split(tempName[0])[1] , tempName[1])

    report(logFile, 
           "\t**Testing \"{}\" in Debug Mode**\n".format(fileName))
    p = subprocess.Popen(os.path.join('.', testExec), cwd=root, 
                             stdout=subprocess.PIPE)
    for line in p.stdout:#for every line of the output
        line = line.decode("utf8")
        line = line.replace("\r\n", "\n")
        print_nonl("\t" + line)
        logFile.write(line)
    #wait till the subprocess ends
    ret = p.poll()
    while(ret is None):
        ret=p.poll()


def runTests(compiler, dynamic, outName, logFile, verbose,
             debug, tests, fail_fast):
    """Runs the Tests of the Refu library

        --compiler: The name of the compiler
        --dynamic: A boolean flag, true for using a dynamic version of
                   the library and false for static
        --outName: The full filename of the compiled refu library with
                   which to link
        --logFile: The log file to keep so that the user can see an
                   overview of how the tests are performed.
        --verbose: A boolean flag denoting whether the function should
                   be verbose about its actions or not
        --debug: A boolean flag denoting whether the tests should be in
                 debug mode (no testing or comparison) or not
        --tests: A list of specific tests to run. If it is empty all
                 tests are run. If it is not then only the specified
                 ones are.
        --fail_fast: A boolean flag denoting if the testing scripts should
                     stop right after a failure or not
    """
    #test executable name is always test for now
    if(platform.system() == 'Windows'):
        testExec = 'test.exe'
    else:
        testExec = 'test'

    libtype_str = "static"
    if(dynamic):
        libtype_str = "dynamic"

    report(logFile,
           "===> Starting Tests for the {} version of Refu library with "
           " the \"{}\" Compiler\n\n".format(libtype_str, compiler))

    # clean the tests directory from object files
    for root, dirs, files in os.walk("."):
        for f in files:
            if(f.endswith(('.o','.obj'))):
                os.remove(os.path.join(root, f))
    # Recursively iterate all the files in the Tests directory
    for root, dirs, files in os.walk("."):
        for f in files:
            fullname = os.path.join(root, f)
            fullname = fullname.replace('.', 'Tests', 1)
            # if it's a test get it's equivalent .c file
            if(fullname.endswith(".expect")):
                part = fullname.partition(".expect")[0]
                # check to see if the user provided specific tests, and if
                # he did check to see if this is one of them
                if(tests):
                    if(os.path.split(part)[1] not in tests):
                        continue
                # compile it
                if(compileTest(part+".c", dynamic, compiler, verbose,
                               logFile) is False):
                    continue#if it fails go to the next test
                # after each compile add a small delay since the
                # subsequent copy may fail
                time.sleep(delay)
                # move the executable to the specific test directory its
                #source was and if there was one there already delete it
                try:
                    os.remove(os.path.join(root, testExec))
                except:
                    pass
                os.rename(os.path.join(".", testExec),
                          os.path.join(root, testExec))
                #deal with the library linking for each test
                configureLinking(dynamic, root, outName)

                #test it
                try:
                    if(debug):
                        debugTest(root, part, logFile, testExec)
                    else:
                        test(root, part, logFile, testExec,
                             verbose, fail_fast)

                except TestError as err:
                    #if there was a test error print it and also check
                    #the library log
                    print(err)
                    check_library_log(logFile, root)
                    if fail_fast:
                        raise TestsFail(
                            "Test \"{}\" failed and --fail"
                            "-fast was requested".format(part))
                except CalledProcessError as err:
                    print("\tThere was an error while trying to run the "
                          "compiled "
                          "executable for test \"{}\"".format(part))
                    print("\tThe output was:")
                    print(err.output)
                    logFile.write("\tTest \"{}\" failed due to "
                                  "inability to run the "
                                  "executable\n".format(part))
                    #also check the library's log
                    check_library_log(logFile, root)
                    if fail_fast:
                        raise TestsFail(
                            "Test \"{}\" failed and --fail"
                            "-hard was requested".format(part))
                finally:
                    # cleaning up
                    if(dynamic):
                        # also if we are testing the dynamic lib,
                        # delete its local copy
                        try:
                            os.remove(os.path.join(root, outName))
                        except:
                            pass
                    #after finishing a test delete the executable
                    try:
                        os.remove(os.path.join(root, testExec))
                    except:
                        pass
                    # adding some delay between tests until I figure out
                    # a better way to do this
                    time.sleep(delay)
    #after all tests have concluded clean up
    print_nonl("\n");
    logFile.write("\n");
    try:
        os.remove(os.path.join('.', testExec))
    except:
        pass
    os.remove(os.path.join('.', outName))
